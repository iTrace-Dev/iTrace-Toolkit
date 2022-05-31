/********************************************************************************************************************************************************
* @file srcmlmapper.cpp
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#include "srcmlmapper.h"

void setLineTextToken(QString source_line, int col, QString syntactic_context, QString& token, QString& token_type) {
    // token_type is currently unused?
    QVector<QString> delimiters = {
        "(",")","{","}",".","++","--","+","-",
        "!","~",",","/","%","*","<<",">>",">>>",
        "<","<=",">","<=","==","!=","@","&","^",
        "|","&&","||","?",":","+=","-=","*=",
        "/=","=",";" // Should we add -> (arrow operator)?
    };
    int start = col,
        end = col;
    token = "";
    // Whitespace
    if(source_line[col].isSpace()) {
        token = "WHITESPACE";
    }
    // COMMENT (look for surrounding whitespace
    else if(syntactic_context.contains("comment")) {
        while(start - 1 >= 0 && !source_line[start-1].isSpace()) { --start; }
        while(end <= source_line.size() - 1 && !source_line[end].isSpace()) { ++end; }
        token = source_line.mid(start,end-start);
    }
    // Operator or a delimiter
    else if(delimiters.contains(QString(source_line[col]))) {
        token = source_line[col];
        while(start - 1 >= 0 && !source_line[start - 1].isSpace() && delimiters.contains(source_line[start-1]+token)) {
            --start;
            token = source_line[start] + token;
        }
        while(end + 1 <= source_line.size() - 1 && !source_line[end].isSpace() && delimiters.contains(token + source_line[end+1])) {
            ++end;
            token += source_line[end];;
        }
    }
    else {
        while(start - 1 >= 0 && !source_line[start-1].isSpace() && !delimiters.contains(QString(source_line[start-1]))) {
            --start;
        }
        while(end <= source_line.size() - 1 && !source_line[end].isSpace() && !delimiters.contains(QString(source_line[end]))) {
            ++end;
        }
        token = source_line.mid(start,end-start);
    }
}


void SRCMLMapper::mapSyntax(SRCMLHandler& srcml, QString unit_path, QString project_path, bool overwrite) {
    QVector<QVector<QString>> responses = idb.getGazesForSyntacticMapping(project_path,overwrite);

    QString unit_data = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n" + srcml.getUnitText(unit_path) + "\n</xml>";


    QDomDocument unit;
    unit.setContent(unit_data,false);

    QVector<QDomElement> elements;
    elements.append(unit.documentElement());
    QDomElement first = unit.documentElement().firstChildElement();

    QVector<QDomElement> parents; parents.push_back(first);
    while(parents.size() != 0) {
        QDomElement crnt = parents[parents.size() - 1];

        elements.push_back(crnt);

        if(!crnt.firstChildElement().isNull()) {
            parents.push_back(crnt.firstChildElement());
        }
        else if(!crnt.nextSiblingElement().isNull()) {
            parents[parents.size() - 1] = crnt.nextSiblingElement();
        }
        else {
            while(parents.size() != 0 && parents[parents.size() - 1].nextSiblingElement().isNull()) {
                parents.pop_back();
            }
            if(parents.size() != 0) { parents[parents.size() - 1] = parents[parents.size() - 1].nextSiblingElement(); }
        }
        QApplication::processEvents();
    }

    std::map<QString,std::pair<QString,QString>> cached_gazes;
    int i = -1;
    for(auto response : responses) {
        ++i;
        int res_line = response[1].toInt(),
            res_col = response[2].toInt();

        QString report = idb.checkAndReturnError();
        if(report != "") { std::cout << "IDB ERROR IN SYNTAX MAPPING: " << report.toUtf8().constData() << std::endl; }

        //THIS CAN CHANGE IN THE FUTURE
        QString gaze_key = project_path + "L" + response[1] + "C" + response[2];

        if(cached_gazes.count(gaze_key) > 0) {
            idb.updateGazeWithSyntacticInfo(response[0],cached_gazes.at(gaze_key).first,cached_gazes.at(gaze_key).second);
            continue;
        }

        QVector<QDomElement> element_list;
        for(auto srcml_element : elements) {
            if(srcml_element.tagName() == "unit") {
                element_list.push_back(srcml_element);
                continue;
            }
            int element_start_line = -1,
                element_start_col = -1,
                element_end_line = -1,
                element_end_col = -1;

            QDomAttr start = srcml_element.attributeNode("pos:start"),
                     end = srcml_element.attributeNode("pos:end");

            //Get pos elements from scrml tag
            if(!start.isNull() && !end.isNull()) {
                if(start.value().contains(":") && !start.value().contains("INVALID_POS")) {
                    element_start_line = start.value().split(":")[0].toInt();
                    element_start_col = start.value().split(":")[1].toInt();
                }
                if(end.value().contains(":") && !end.value().contains("INVALID_POS")) {
                    element_end_line = end.value().split(":")[0].toInt();
                    element_end_col = end.value().split(":")[1].toInt();
                }
            }
            else { continue; } // element doesn't have position info

            // Check for bugs in srcml
            if(element_end_line < element_start_line || element_end_line < 0 || element_start_line < 0) { continue; }
            // No more tags can encompass the token
            if(element_start_line > res_line) { break; }
            // No tags on this line can encompass token
            if(res_line == element_start_line && element_start_col > res_col) { break; }
            // Skip this tag since it can't encompass token
            if(res_line > element_start_line && res_line > element_end_line) { continue; }
            // In between multiple lines
            if(res_line >= element_start_line && res_line < element_end_line) {
                element_list.push_back(srcml_element);
            }
            else if(res_line >= element_start_line && res_line == element_end_line) {
                if(res_col <= element_end_col) { element_list.push_back(srcml_element);  }
                else { continue; }
            }
            QApplication::processEvents();
        }

        QString syntactic_context = "",
                xpath = "/";
        for(auto element : element_list) {
            if(element.namespaceURI() == "") {
                xpath += "/src:"+element.tagName();
            }
            else {
                xpath += "/" + element.tagName();
            }

            if(element.tagName() == "unit") {
                xpath += "[@filename=\"" + element.attributeNode("filename").value()+"\"]";
            }
            QDomAttr start = element.attributeNode("pos:start"),
                     end = element.attributeNode("pos:end");
            if(!start.isNull() && !end.isNull()) {
                xpath += "[@pos:start=\""+start.value()+"\" and ";
                xpath += "@pos:end=\""+end.value()+"\"]";
            }
            if(syntactic_context != "") {
                syntactic_context += "->"+element.tagName();
            }
            else {
                syntactic_context = element.tagName();
            }
            QApplication::processEvents();
        }
        cached_gazes.emplace(gaze_key,std::make_pair(xpath,syntactic_context));
        idb.updateGazeWithSyntacticInfo(response[0],xpath,syntactic_context);
    }
}

void SRCMLMapper::mapToken(SRCMLHandler& srcml, QString unit_path, QString project_path, bool overwrite) {
    QVector<QVector<QString>> responses = idb.getGazesForSourceMapping(project_path,overwrite);

    QString report = idb.checkAndReturnError();
    if(report != "") { std::cout << "IDB ERROR IN TOKEN MAPPING TOP: " << report.toUtf8().constData() << std::endl; }

    QStringList unit_body = srcml.getUnitBody(unit_path).split("\n");
    //std::cout << "UNIT BODY SIZE: " << unit_body.size() << std::endl;;
    //for(auto i : unit_body) { std::cout << i.toUtf8().constData() << std::endl; }

    // Check if any of the values are 0 - if they are, the data is 0-indexed and doesn't need to be shifted.
    // This is NOT an exhaustive check, plugins should by default be 1-indexed. This is only a small check.
    bool one_indexed = true;
    for(auto response : responses) {
        if(response[1].toInt() == 0 || response[2].toInt() == 0) {
            one_indexed = false;
            break;
        }
    }

    std::map<QString,std::pair<QString,QString>> cached_gazes;
    //std::cout << "RESPONSES SIZE TOKEN: " << responses.size() << std::endl;
    for(auto response : responses) {
        int res_line = response[1].toInt(),
            res_col = response[2].toInt();
        if(one_indexed) { --res_line; --res_col; }
        QString token = "",
                token_type = "";

        QString report = idb.checkAndReturnError();
        if(report != "") { std::cout << "IDB ERROR IN TOKEN MAPPING BOTTOM: " << report.toUtf8().constData() << std::endl; }

        QString gaze_key = project_path+"L"+response[1]+"C"+response[2];
        if(cached_gazes.count(gaze_key) > 0) {
            idb.updateGazeWithTokenInfo(response[0],cached_gazes.at(gaze_key).first,cached_gazes.at(gaze_key).second);
            continue;
        }
        // If line_num > number of line in body
        // Invalid, assume it is whitespace
        // OR
        // If the col position is outside the bounds of the line
        // Invalid, assume it is whitespace
        if(!(res_line < unit_body.size()) || !(res_col < unit_body[res_line].size())) {
            token = "WHITESPACE";
            cached_gazes.insert(std::make_pair(gaze_key,std::make_pair(token,token_type)));
            idb.updateGazeWithTokenInfo(response[0],token,token_type);
            continue;
        }
        setLineTextToken(unit_body[res_line],res_col,response[3],token,token_type);

        cached_gazes.insert(std::make_pair(gaze_key,std::make_pair(token,token_type)));
        idb.updateGazeWithTokenInfo(response[0],token,token_type);
        QApplication::processEvents();
    }
}
