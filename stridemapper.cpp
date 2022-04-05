#include "stridemapper.h"

void StrideMapper::mapSyntax(SRCMLHandler& srcml, QString unit_path, QString project_path, bool overwrite) {
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
