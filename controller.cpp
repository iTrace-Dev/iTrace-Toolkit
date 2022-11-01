/********************************************************************************************************************************************************
* @file controller.cpp
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#include "controller.h"

/////////////////////////////////////////
// HELPERS
/////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const QString& s) { out << s.toUtf8().constData(); return out; }

void changeFilePathOS(QString& path) {
    #ifdef Q_OS_WIN
        path.remove("file:///");
    #else
        path.remove("file://");
    #endif
}

void findAllGazeLeadingElements(QVector<QDomElement>& list, QDomNode crnt, const int& res_line, const int& res_col, bool& cont) {
    if(crnt.isNull() || !cont) { return; }
    QDomElement elem = crnt.toElement();
    if(elem.isNull()) { return; }
    int element_start_line = -1,
        element_start_col = -1,
        element_end_line = -1,
        element_end_col = -1;
    if(!elem.attributeNode("start").isNull() && !elem.attributeNode("end").isNull() && cont) {
        QString start = elem.attributeNode("start").value();
        if(start.contains(":") && !start.contains("INVALID_POS")) {
            element_start_line = start.split(":")[0].toInt();
            element_start_col = start.split(":")[1].toInt();

            QString end = elem.attributeNode("end").value();
            if(end.contains(":") && !end.contains("INVALID_POS")) {
                element_end_line = end.split(":")[0].toInt();
                element_end_col = end.split(":")[1].toInt();
            }

            //Check for bugs
            if(element_end_line < element_start_line || element_end_line < 0 || element_start_line < 0) {
                return;
            } // Skip this token

            // Check if additional tags can encompass
            if(element_start_line > res_line || (element_start_line == res_line && element_start_col > res_col)) {
                cont = false; // No more can be found, stop all
                return;
            }

            if(res_line > element_start_line && res_line > element_end_line) {
                return; // Skip this tag
            }

            if(res_line >= element_start_line && res_line < element_end_line) {
                list.push_back(elem);
            }
            else if(res_line >= element_start_line && res_line == element_end_line && res_col <= element_end_col) {
                list.push_back(elem);
            }
            else { return; }

            QDomNode n = elem.firstChild();
            while(!n.isNull()) {
                findAllGazeLeadingElements(list,n,res_line,res_col,cont);
                if(!cont) { return; }
                n = n.nextSibling();
            }
        }
    }
}

QString getFilenameFromXpath(QString xpath) {
    int start = xpath.indexOf("'"),
        end = xpath.lastIndexOf("'");
    QString filename = xpath.mid(start,end-start+1);
    filename = filename.mid(filename.length() - filename.lastIndexOf("/"));
    return filename;
}

/////////////////////////////////////////
// CLASS METHODS
/////////////////////////////////////////

Controller::Controller(QObject* parent) : QObject(parent) {}

void Controller::saveDatabaseFile(QString file_loc) {
    closeDatabase();
    changeFilePathOS(file_loc);
    std::ofstream file;
    file.open(file_loc.toUtf8().constData());
    if(!file.is_open()) {
        emit outputToScreen("#FF0000","Failed to create database");
        emit warning("ERROR","A problem was encountered when trying to create the file.");
        log->writeLine("ERROR","Could not create database at " + file_loc);
        return;
    }
    file.close();
    idb = Database(file_loc);

    log->writeLine("INFO","Successfully created new database " + file_loc);

    emit outputToScreen("black","Successfully created database");
    emit databaseSet(file_loc);
}

void Controller::loadDatabaseFile(QString file_path) {
    closeDatabase();
    changeFilePathOS(file_path);
    idb = Database(file_path);

    for(auto i : idb.getSessions()) { emit taskAdded(i); }

    log->writeLine("INFO","Successfully loaded database " + file_path);

    emit outputToScreen("black","Successfully loaded database.");
    emit databaseSet(file_path);
}

void Controller::closeDatabase() {
    idb.close();

    log->writeLine("INFO","Closed currently loaded database");

    emit databaseClosed();
}

void Controller::importXMLFile(QString file_path) {
    changeFilePathOS(file_path);

    if(!idb.isDatabaseOpen()) {
        log->writeLine("ERROR","Database not opened: Import XML");
        emit warning("Database Error","There is no Database currently loaded.");
        return;
    }

    //Exit function if file is already imported
    if(idb.fileExists(QCryptographicHash::hash(file_path.toUtf8().constData(),QCryptographicHash::Sha1).toHex())) {
        log->writeLine("ERROR","XML file already imported "+file_path);
        emit warning("XML Import Error","This XML File has already been imported into the Database.");
        return;
    }

    XMLHandler xml_file(file_path);
    QString type = xml_file.getXMLFileType();

    if(type == "itrace_core") { importCoreXML(file_path); }
    else if(type == "itrace_plugin") { importPluginXML(file_path); }
    else {
        log->writeLine("WARNING","Unrecognized iTrace XML file "+file_path);
        emit outputToScreen("#F55904","Unrecognized iTrace XML file: "+file_path);
    }
}

void Controller::importDatabaseFile(QString file_path) {
    changeFilePathOS(file_path);

    std::cout << "Importing DB : " << file_path << std::endl;

    idb.importExistingDatabase(file_path);

    for(auto i : idb.getSessions()) { emit taskAdded(i); }

    log->writeLine("INFO","Successfully imported database " + file_path);
    emit outputToScreen("black","Successfully imported database.");

}

void Controller::batchAddXML(QString folder_path) {
    log->writeLine("INFO","Scanning and adding all XML files in: "+folder_path);
    if(!idb.isDatabaseOpen()) {
        log->writeLine("ERROR","Database not opened: Batch add XML files");
        emit warning("Database Error","There is no Database currently loaded.");
        return;
    }
    emit setProgressBarToIndeterminate();

    changeFilePathOS(folder_path);

    QDirIterator counter(folder_path, QDir::Files, QDirIterator::Subdirectories);
    int count = 0;

    while(counter.hasNext()) {
        ++count;
        counter.next();
        if(count > 1000) {
            emit stopProgressBar();
            emit warning("File Search Too Large","The selected folder contained too many sub items.");
            return;
        }
    }


    std::map<QString,std::pair<std::vector<QString>,bool>> files;
    QDirIterator dir(folder_path, QStringList() << "*.xml", QDir::Files, QDirIterator::Subdirectories);;
    while(dir.hasNext()) {
        QString filename = dir.next();
        //if(filename.endsWith(".xml")) {
        changeFilePathOS(filename);
        XMLHandler xml_file(filename);
        QString type = xml_file.getXMLFileType();
        if(type == "itrace_core" || type == "itrace_plugin") {
            QString id = xml_file.getElementAttribute("session_id");
            if(files.count(id) == 0) {
                auto insert = files.emplace(id,std::make_pair(std::vector<QString>(),false));
                insert.first->second.first.push_back(filename);
            }
            else { (files.find(id))->second.first.push_back(filename); }
            if(type == "itrace_core") { files.find(id)->second.second = true; }
        }
        //}
        QApplication::processEvents();
    }
    QString badPairWarn, alreadyInWarn;
    for(auto i = files.begin(); i != files.end(); ++i) {
        if(i->second.first.size() >= 2 && i->second.second) {
            for(auto j : i->second.first) {
                if(!idb.fileExists(QCryptographicHash::hash(j.toUtf8().constData(),QCryptographicHash::Sha1).toHex())) { importXMLFile(j); }
                else { alreadyInWarn += "\t" + j + "\n"; }
            }
        }
        else { for(auto j : i->second.first) { badPairWarn += "\t" + j + "\n"; } }
        QApplication::processEvents();
    }

    emit stopProgressBar();

    QString warn;
    if(badPairWarn != "") { warn += "The following files were missing their associated pair file:\n" + badPairWarn; }
    if(alreadyInWarn != "") { warn += "The following files are already imported:\n" + alreadyInWarn; }
    if(warn != "") {
        log->writeLine("ERROR","Batch XML Import Errors: "+warn);
        emit warning("XML Import Error",warn);
    }
}

void Controller::importCoreXML(const QString& file_path) {
    if(!idb.isDatabaseOpen()) {
        log->writeLine("ERROR","Database not opened: Core File import");
        emit warning("Database Error","There is no Database currently loaded.");
        return;
    }

    //log->writeLine("INFO","Importing iTrace Core file: "+file_path);

    QElapsedTimer time;
    time.start();

    XMLHandler core_file(file_path);

    idb.startTransaction();

    QString session_id,
            participant_id,
            task_name,
            session_date_time,
            calibration_id = "-1",
            calibration_x,
            calibration_y,
            calibration_point_id;
    bool skipCalibration = true;

    while(!core_file.isAtEnd()) {
        QString element = core_file.getNextElementName();

        if(element == "itrace_core") {
            session_id = core_file.getElementAttribute("session_id");
            participant_id = core_file.getElementAttribute("participant_id");
            task_name = core_file.getElementAttribute("task_name");
            session_date_time = core_file.getElementAttribute("session_date_time");

            // Insert participant only if it doesn't already exist
            if(!idb.participantExists(participant_id)) {
                idb.insertParticipant(participant_id,"null"); // The null may change in the future
            }

            // Insert file
            idb.insertFile(QCryptographicHash::hash(core_file.getFilePath().toUtf8().constData(),QCryptographicHash::Sha1).toHex(),session_id,core_file.getFilePath(),"core");
        }
        else if(element == "environment") {
            // Insert session
            idb.insertSession(session_id,participant_id,core_file.getElementAttribute("screen_width"),core_file.getElementAttribute("screen_height"),core_file.getElementAttribute("tracker_type"),core_file.getElementAttribute("tracker_serial_number"),session_date_time,session_date_time,core_file.getElementAttribute("screen_recording_start"),task_name);
        }
        else if(element == "calibration") {
            calibration_id = core_file.getElementAttribute("timestamp");
            if(!idb.calibrationExists(calibration_id)) {
                // Insert calibration
                idb.insertCalibration(calibration_id);
                skipCalibration = false;
            }
            else { skipCalibration = true; }
        }
        else if(element == "calibration_point" && !skipCalibration) {
            // Ternary operators convert "0" to "0.0" for better formating and consistent hashing
            calibration_x = core_file.getElementAttribute("x") == "0" ? "0.0" : core_file.getElementAttribute("x");
            calibration_y = core_file.getElementAttribute("y") == "0" ? "0.0" : core_file.getElementAttribute("y");
            calibration_point_id = QCryptographicHash::hash((calibration_id+calibration_x+calibration_y).toUtf8().constData(),QCryptographicHash::Sha1).toHex();
            // Insert calibration_point
            idb.insertCalibrationPoint(calibration_point_id,calibration_id,calibration_x,calibration_y);
        }
        else if(element == "sample" && !skipCalibration) {
            // Insert calibration_sample
            idb.insertCalibrationSample(calibration_point_id,core_file.getElementAttribute("left_x"),core_file.getElementAttribute("left_y"),core_file.getElementAttribute("left_validity"),core_file.getElementAttribute("right_x"),core_file.getElementAttribute("right_y"),core_file.getElementAttribute("right_validity"));
        }
        else if(element == "response") {
            // Insert gaze
            idb.insertGaze(core_file.getElementAttribute("event_id"),session_id,calibration_id,participant_id,core_file.getElementAttribute("tracker_time"),core_file.getElementAttribute("core_time"),core_file.getElementAttribute("x"),core_file.getElementAttribute("y"),core_file.getElementAttribute("left_x"),core_file.getElementAttribute("left_y"),core_file.getElementAttribute("left_pupil_diameter"),core_file.getElementAttribute("left_validation"),core_file.getElementAttribute("right_x"),core_file.getElementAttribute("right_y"),core_file.getElementAttribute("right_pupil_diameter"),core_file.getElementAttribute("right_validation"),core_file.getElementAttribute("user_left_x"),core_file.getElementAttribute("user_left_y"),core_file.getElementAttribute("user_left_z"),core_file.getElementAttribute("user_right_x"),core_file.getElementAttribute("user_right_y"),core_file.getElementAttribute("user_right_z"));
        }
        /*QString report = idb.checkAndReturnError();
        if(report != "") {
            log->writeLine("WARNING","The followng SQLite Error occured while handling core file: "+report);
        }*/
        QString report = core_file.checkAndReturnError();
        if(report != "") {
            log->writeLine("WARNING","The following XML Error occured while handling core file: "+report);
        }
        QApplication::processEvents();
    }

    idb.commit();

    log->writeLine("INFO","Core file successfully imported "+file_path);

    emit taskAdded(participant_id + " - " + task_name);
    emit outputToScreen("black",QString("Core file imported. Took %1 seconds").arg(time.elapsed() / 1000.0));

}

void Controller::importPluginXML(const QString& file_path) {
    if(!idb.isDatabaseOpen()) {
        log->writeLine("ERROR","Database not opened: Plugin File import");
        emit warning("Database Error","There is no Database currently loaded.");
        return;
    }

    QElapsedTimer time;
    time.start();

    XMLHandler plugin_file(file_path);

    idb.startTransaction();

    QString session_id,
            ide_plugin_type;

    // Used for checking for duplicate data
    QVector<QString> all_ids;// = idb.getAllIDEContextIDs();

    while(!plugin_file.isAtEnd()) {
        QString element = plugin_file.getNextElementName();
        //std::cout << element << std::endl;

        if(element == "itrace_plugin") {
            session_id = plugin_file.getElementAttribute("session_id");
        }
        else if(element == "environment") {
            std::map<QString,QString> types;
            types["msvs"] = "vstudio_plugin";
            types["eclipse"] = "eclipse_plugin";
            types["atom"] = "atom_plugin";
            types["chrome"] = "chrome_plugin";
            ide_plugin_type = plugin_file.getElementAttribute("plugin_type").toLower();
            // Insert file
            idb.insertFile(QCryptographicHash::hash(plugin_file.getFilePath().toUtf8().constData(),QCryptographicHash::Sha1).toHex(),session_id,plugin_file.getFilePath(),types.at(ide_plugin_type));
        }
        else if(element == "response") {
            // Insert ide_context

            // Check if we are inserting duplicate data
            //if(idb.pluginResponseExists(plugin_file.getElementAttribute(("event_id")))) {
            if (all_ids.contains(plugin_file.getElementAttribute("event_id"))) {
                QString output = "Duplicate Plugin Context data in file: "+file_path+" with event_id: " + plugin_file.getElementAttribute(("event_id"));
                emit outputToScreen("#F55904",output);
                emit outputToScreen("#F55904",plugin_file.getElementAttribute(("event_id")));
                log->writeLine("WARNING",output);
                continue;
            }

            // The last 4 parameters are unused for the moment
            idb.insertIDEContext(plugin_file.getElementAttribute("event_id"),session_id,plugin_file.getElementAttribute("plugin_time"),ide_plugin_type,plugin_file.getElementAttribute("gaze_target"),plugin_file.getElementAttribute("gaze_target_type"),plugin_file.getElementAttribute("source_file_path"),plugin_file.getElementAttribute("source_file_line"),plugin_file.getElementAttribute("source_file_col"),plugin_file.getElementAttribute("editor_line_height"),plugin_file.getElementAttribute("editor_font_height"),plugin_file.getElementAttribute("editor_line_base_x"),plugin_file.getElementAttribute("editor_line_base_y"),"","","","",plugin_file.getElementAttribute("x"),plugin_file.getElementAttribute("y"));
            all_ids.push_back(plugin_file.getElementAttribute("event_id"));
        }
        /*QString report = idb.checkAndReturnError();
        if(report != "") {
            log->writeLine("WARNING","The followng SQLite Error occured while handling plugin file: "+report);
        }*/
        QString report = plugin_file.checkAndReturnError();
        if(report != "") {
            log->writeLine("WARNING","The following XML Error occured while handling plugin file: "+report);
        }
        QApplication::processEvents();
    }

    idb.commit();

    log->writeLine("INFO","Plugin file successfully imported");
    emit outputToScreen("black",QString("Plugin file imported. Took %1 seconds").arg(time.elapsed() / 1000.0));
}

void Controller::generateFixationData(QVector<QString> tasks, QString algSettings) {
    log->writeLine("INFO","Generating fixations with settings: "+algSettings);

    QElapsedTimer time;
    time.start();

    int counter = 0;

    std::vector<QString> sessions;
    for(auto i : tasks) { // Get the sessions that the user wants to use
        QStringList values = i.split(" - ");
        if(values[2] == "1") {
            sessions.push_back(idb.getSessionFromParticipantAndTask(values[0],values[1]));
            counter += idb.getGazeTargetsFromSession(sessions.back()).size();
        }
    }

    emit startProgressBar(0,counter);

    idb.startTransaction();
    counter = 1;

    for(auto session_id : sessions) {
        //std::cout << "?" << std::endl;
        QVector<Fixation> session_fixations;
        QVector<QString> gaze_targets = idb.getGazeTargetsFromSession(session_id);
        QString fixation_filter_settings;
        for(auto gaze_target : gaze_targets) {
            //emit outputToScreen("black","Generating fixations for gaze_target: "+gaze_target);
            QVector<Gaze> gazes = idb.getGazesFromSessionAndTarget(session_id,gaze_target);
            if(gazes.length() == 0) {
                continue;
            }
            FixationAlgorithm* algorithm;
            QStringList settings = algSettings.split("-");
            if(settings[0] == "BASIC") {
                //BASIC-4-35-40 = BASIC-window_size-radius-peak
                algorithm = new BasicAlgorithm(gazes,settings[window_size].toInt(),settings[radius].toInt(),settings[peak].toInt());
            }
            else if(settings[0] == "IDT") {
                //IDT-10-125 = IDT-duration_window-dispersion
                algorithm = new IDTAlgorithm(gazes,settings[duration_window].toInt(),settings[dispersion].toInt());
            }
            else if(settings[0] == "IVT") {
                //IVT-50-80 = IVT-velocity-duration
                algorithm = new IVTAlgorithm(gazes,settings[velocity].toInt(),settings[duration].toInt());
            }
            else { emit warning("Algorithm Error","An invalid algorithm type was supplied: " + settings[0]); return; } // Error handler
            session_fixations.append(algorithm->generateFixations());
            fixation_filter_settings = algorithm->generateFixationSettings();
            emit setProgressBarValue(counter); ++counter;
            QApplication::processEvents();
            delete algorithm;
        }
        for(auto item = session_fixations.begin(); item != session_fixations.end(); ++item) {
            item->calculateDatabaseFields();
        }
        std::sort(session_fixations.begin(), session_fixations.end(), [](const Fixation& a, const Fixation& b) -> bool { return a.fixation_event_time > b.fixation_event_time; });
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        QString fixation_run_id = QString::number(ms.count());
        QString fixation_date_time = fixation_run_id; // This will probably be changed in the future
        idb.insertFixationRun(fixation_run_id,session_id,fixation_date_time,fixation_filter_settings);

        int fixation_order = 1;
        for(auto fix = session_fixations.begin(); fix != session_fixations.end(); ++fix) {
            QString fixation_id = QUuid::createUuid().toString();
            fixation_id.remove("{"); fixation_id.remove("}");
            idb.insertFixation(fixation_id,fixation_run_id,QString::number(fix->fixation_event_time),QString::number(fixation_order),QString::number(fix->x),QString::number(fix->y),fix->target,QString::number(fix->source_file_line),QString::number(fix->source_file_col),fix->token == "" ? "null" : "\""+fix->token+"\"",fix->syntactic_category == "" ? "null" : "\""+fix->syntactic_category+"\"",fix->xpath == "" ? "null" : "\""+fix->xpath+"\"",QString::number(fix->left_pupil_diameter),QString::number(fix->right_pupil_diameter),QString::number(fix->duration));


            ++fixation_order;
            std::set<long long> unique_gazes; // What does this even do? Check the py
            for(auto gaze : fix->gaze_vec) {
                if(unique_gazes.find(gaze.event_time) != unique_gazes.end()) { continue; }
                idb.insertFixationGaze(fixation_id,QString::number(gaze.event_time));
            }
        }
        QApplication::processEvents();
    }

    idb.commit();
    emit stopProgressBar();
    emit outputToScreen("black",QString("Fixation data generated. Elapsed time: %1").arg(time.elapsed() / 1000.0));
}

void Controller::mapTokens(QString srcml_file_path, QVector<QString> tasks, bool overwrite = true) {
    QElapsedTimer timer;
    timer.start();

    QVector<QString> sessions;
    for(auto i : tasks) { // Get the sessions that the user wants to use
        QStringList values = i.split(" - ");
        if(values[2] == "1") {
            sessions.push_back(idb.getSessionFromParticipantAndTask(values[0],values[1]));
        }
    }

    changeFilePathOS(srcml_file_path);

    SRCMLHandler srcml(srcml_file_path);
    if(!srcml.isPositional()) {
        emit warning("srcML Error","The provided srcML File does not contain positional data. Tokens will not be mapped without it. Re-generate the srcML Archive file with the --position flag");
        return;
    }
  
    // Add srcML Archive to Files table
    if(!idb.fileExists(QCryptographicHash::hash(srcml.getFilePath().toUtf8().constData(),QCryptographicHash::Sha1).toHex())) {
        idb.insertFile(QCryptographicHash::hash(srcml.getFilePath().toUtf8().constData(),QCryptographicHash::Sha1).toHex(),"null",srcml.getFilePath(),"srcml_archive");
    }

    QVector<QString> all_files;
    if (srcml_file_path.endsWith(".stride",Qt::CaseInsensitive))
        all_files.append(srcml_file_path);
    else
        all_files = srcml.getAllFilenames();

    idb.startTransaction();

    QVector<std::pair<QString,QString>> files_viewed = idb.getFilesViewed();

    emit startProgressBar(0,files_viewed.size());
    int counter = 1;
    emit outputToScreen("black","Mapping tokens for "+QString::number(files_viewed.size())+" gaze targets.");
    emit outputToScreen("black","This could take a while. Please wait.");

    QString warn = "";
    SRCMLMapper mapper(idb);
    StrideMapper strideMapper(idb);
    for(auto file = files_viewed.begin(); file != files_viewed.end(); file++) {
        QElapsedTimer inner_timer;
        inner_timer.start();
        if(!file->second.isNull() && !file->second.isEmpty()) {
            QString unit_path = findMatchingPath(all_files,file->second);
            if(unit_path == "") {
                warn += "\n" + file->second;
                emit outputToScreen("#F55904",QString("Target %1 skipped - no valid unit.").arg(counter));
                emit setProgressBarValue(counter); ++counter;
                continue;
            }

            if (file->second.endsWith(".stride")) {
                strideMapper.mapSyntax(unit_path, file->second, overwrite);
            }
            else {
                mapper.mapSyntax(srcml,unit_path,file->second,overwrite,sessions);
                mapper.mapToken(srcml,unit_path,file->second,overwrite,sessions);
            }
        }
        emit outputToScreen("black",QString("%1 / %2 Targets Mapped. Time elasped: %3").arg(counter).arg(files_viewed.size()).arg(inner_timer.elapsed() / 1000.0));
        emit setProgressBarValue(counter); ++counter;
        QApplication::processEvents();
    }

    idb.commit();
    emit stopProgressBar();
    emit outputToScreen("black",QString("Token Mapping done. Time elasped: %1").arg(timer.elapsed() / 1000.0));
    if(warn != "") {
        warn = "The following gaze targets had no matching unit:" + warn;
        emit warning("Token Mapping Error",warn);
    }
    else {
        QApplication::beep();
    }
}

// This should probably be a helper function
QString Controller::findMatchingPath(QVector<QString> all_files, QString file) {
    file.replace("\\","/");
    file = file.toLower();
    QVector<QStringList> possible;
    QStringList file_split = file.split("/");
    QString check = file_split[file_split.size()-1];
    for(auto i : all_files) {
        if(i.toLower().endsWith(check)) { possible.push_back(i.split("/")); }
    }
    if(possible.size() == 0) { return ""; }
    else if(possible.size() == 1) { return possible[0].join("/"); }

    QString shortest = "";
    int passes = 1;


    while(possible.size() != 1) {
        QVector<QStringList> candidates;
        if(passes > file_split.size()) { return shortest; }
        for(auto unit_path : possible) {
            if(passes > unit_path.size()) {
                if(shortest == "") { shortest = unit_path.join("/"); }
                continue;
            }
            QString unit_check = unit_path[unit_path.size() - passes].toLower();
            QString file_check = file_split[file_split.size() - passes];
            if(unit_check == file_check) {
                candidates.push_back(unit_path);
            }
        }
        possible = candidates;
        ++passes;
        QApplication::processEvents();
    }
    if(possible.size() == 0) { return ""; }
    return possible[0].join("/");
}


void Controller::highlightFixations(QString dir, QString srcml_file_path) {
    /*if(!idb.isDatabaseOpen()) {
        emit warning("Database Error","There is no Database currently loaded.");
        return;
    }

    QElapsedTimer timer;
    timer.start();

    emit outputToScreen("black","Highlighting Fixations...");
    QVector<QString> ids = idb.getFixationRunIDs();
    for(auto id : ids) {
        emit outputToScreen("black","Fixation Run: " + id);
        highlightTokens(idb.getFixationsFromRunID(id),SRCMLHandler(srcml_file_path),dir,id);
    }
    emit outputToScreen("black",QString("Done Highlighting! Time elapsed: %1").arg(timer.elapsed() / 1000.0));*/
}

// WIP
void Controller::highlightTokens(QVector<QVector<QString>> fixations, SRCMLHandler srcml, QString dir, QString run_id) {
    /*//xpath -> [source_file_line,source_file_col,token]
    //mkdir((dir+"/"+run_id).toUtf8().constData());

    std::map<QString,QVector<QVector<QString>>> token_map;
    for(auto fixation : fixations) {
        QString unit_xpath = fixation[0].left(fixation[0].indexOf("]")) + "]";
        QVector<QString> extra = { fixation[1],fixation[2],fixation[3] };
        //token_map.insert(std::make_pair(unit_xpath,extra));
        if(token_map.count(unit_xpath) == 0) {
            token_map.insert(std::make_pair(unit_xpath,QVector<QVector<QString>>()));
        }
        token_map.find(unit_xpath)->second.push_back(extra);
        std::cout << "("; for(auto i : extra) { std::cout << i << ","; } std::cout << ")" << std::endl;
    }
    for(auto unit : token_map) {
        std::sort(unit.second.begin(), unit.second.end(), [](const QVector<QString>& a,const QVector<QString>& b) -> bool { return a[0].toInt() < b[0].toInt() && a[1].toInt() < b[1].toInt(); });
        generateHighlightedFile(dir,getFilenameFromXpath(unit.first),srcml.getUnitText(unit.first).split("\n"),unit.second);
    }
    // TODO - Need to add the run_prettify.js file
    // Currently don't know how to have that work
    // with how C++ creates executables

    //emit outputToScreen("Finished Exporting Files");*/

}

// TODO - NOT YET DONE
void Controller::generateHighlightedFile(QString dir, QString filename, QStringList source_contents, QVector<QVector<QString>> locations) {
    /*emit outputToScreen("Processing: " + filename);

    std::set<QString> set_keys;
    // This is a sin against man
    // Fix this later \/
    std::map<int,QVector<std::pair<std::pair<int,int>,QString>>> boundary_matches;
    
    for(auto location : locations) {
        int line_num = location[0].toInt() - 1,
            col_num = location[1].toInt() - 1;
        QString token = location[2],
                source_line = source_contents[line_num];
        std::cout << "SOURCE LINE: " << source_line << std::endl;
        QString set_key = token + "L" + QString::number(line_num) + "C" + QString::number(col_num);

        if(set_keys.count(set_key) > 0) { continue; }
        else { set_keys.insert(set_key); }
        
        bool bounds_hit = false;
        if(boundary_matches.count(line_num) > 0) {
            for(auto matches : boundary_matches.at(line_num)) {
                bounds_hit = col_num >= matches.first.first && matches.first.second && matches.second == token;
                if(bounds_hit) { break; }
            }
        }
        else { boundary_matches.insert(std::make_pair(line_num,QVector<std::pair<std::pair<int,int>,QString>>())); }
        
        if(bounds_hit) { continue; }
        
        if(token.size() == 1) {
            int token_index = source_line.indexOf(token,col_num);
            source_line = source_line.left(token_index) + "<MARK>" + token + "</MARK>" + source_line.right(source_line.size() - token_index - 1);
            boundary_matches.at(line_num).push_back(std::make_pair(std::make_pair(token_index,token_index),token));
        }
        else {
            int start_index = source_line.indexOf(token),
                end_index = start_index + token.size() - 1;
            while(!(col_num >= start_index && col_num <= end_index)) {
                start_index = source_line.indexOf(token,start_index+1);
                end_index = start_index + token.size() - 1;
            }
            boundary_matches.at(line_num).push_back(std::make_pair(std::make_pair(start_index,end_index),token));
            source_line = source_line.left(start_index) + "<MARK>" + token +  "</MARK>" + source_line.right(source_line.size() - end_index);
        }
        source_contents[line_num] = source_line;
    }
    QString output = dir + filename.left(filename.lastIndexOf(".")) + "-" + QString::number(std::time(nullptr))+".html";
    //std::ifstream highlighted_file(output.toUtf8().constData());
    QFile highlighted_file(output);
    highlighted_file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream highlighted_file_stream(&highlighted_file);
    highlighted_file_stream << "<HTML><HEAD><SCRIPT src=\"run_prettify.js\"></SCRIPT><TITLE>" << filename
                            << "</TITLE></HEAD><BODY>\n<PRE><CODE class=\"prettyprint\">\n";
    for(auto line_of_code : source_contents) {
        line_of_code.replace("<","&lt;");
        line_of_code.replace(">","&gt;");
        line_of_code.replace("&lt;MARK&gt;","<MARK>");
        line_of_code.replace("&lt;/MARK&gt;","</MARK>");
        highlighted_file_stream << line_of_code;
    }
    highlighted_file_stream << "</CODE></PRE>\n</BODY></HTML>\n";
    */
}

QString Controller::generateQuery(QString targets, QString token_types, QString duration_min, QString duration_max, QString source_file_line_min, QString source_file_line_max, QString source_file_col_min, QString source_file_col_max, QString right_pupil_diameter_min, QString right_pupil_diameter_max, QString left_pupil_diameter_min, QString left_pupil_diameter_max) {
    QString query = "SELECT * FROM fixation WHERE ";

    // Add in Targets
    if(targets != "") {
        QStringList target_list = targets.split(',');
        query += QString("fixation_target = \"%1\"").arg(target_list[0]);
        for(auto i = target_list.begin()+1; i != target_list.end(); ++i) {
            query += QString(" OR fixation_target = \"%1\" ").arg(*i);
        }
        query += " AND ";
    }
    // Add in Tokens
    if(token_types != "") {
        QStringList token_list = token_types.split(',');
        query += QString("(fixation_target = \"%1\"").arg(token_list[0]);
        for(auto i = token_list.begin()+1; i != token_list.end(); ++i) {
            query += QString(" OR token = \"%1\" ").arg(*i);
        }
        query += ") AND ";
    }

    // Duration
    query += " duration >= " + duration_min + " " + (duration_max.toInt() == -1 ? "" : "AND duration <= "+duration_max);
    // Source File Line #
    query += " AND source_file_line >= " + source_file_line_min + " " + (source_file_line_max.toInt() == -1 ? "" : "AND source_file_line <= "+source_file_line_max);
    // Source File Column #
    query += " AND source_file_col >= " + source_file_col_min + " " + (source_file_col_max.toInt() == -1 ? "" : "AND source_file_col <= "+source_file_col_max);


    // Left and Right Pupil Diameters
    query += " AND left_pupil_diameter >= " + left_pupil_diameter_min + " AND left_pupil_diameter <= " + left_pupil_diameter_max + " ";
    query += " AND right_pupil_diameter >= " + right_pupil_diameter_min + " AND right_pupil_diameter <= " + right_pupil_diameter_max + " ";

    std::cout << "QUERY: " << query << "||\n" << std::endl;
    return query;
}

void Controller::loadQueryFile(QString file_path, QString output_type, QString output_url) {
    changeFilePathOS(file_path);
    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit outputToScreen("red","No file matching the given path was found.");
        return;
    }
    QTextStream stream(&file);
    QString data = stream.readAll();

    file.close();

    generateQueriedData(data,output_type,output_url);
}

void Controller::saveQueryFile(QString query, QString file_path) {
    changeFilePathOS(file_path);
    std::ofstream file(file_path.toUtf8().constData());
    file << query;
    file.close();
}

void Controller::generateQueriedData(QString query, QString output_type, QString output_url) {
    QVector<QVector<QString>> data = idb.runFilterQuery(query);
    QString safeQuery = query.replace("\"", "\\\"");
    QString savename = output_url+"/fixation_query_"+QString::number(std::time(nullptr))+output_type;
    changeFilePathOS(savename);
    std::cout << savename << std::endl;
    /////// DATABASE
    if(output_type == ".db3") {
        QSqlDatabase output = QSqlDatabase::addDatabase("QSQLITE","output");
        output.setDatabaseName(savename);
        output.open();
        output.exec("CREATE TABLE IF NOT EXISTS fixation(fixation_id TEXT PRIMARY KEY,fixation_run_id INTEGER,fixation_start_event_time INTEGER,fixation_order_number INTEGER,x INTEGER,y INTEGER,fixation_target TEXT,source_file_line INTEGER, source_file_col INTEGER,token TEXT,syntactic_category TEXT,xpath TEXT,left_pupil_diameter REAL,right_pupil_diameter REAL,duration INTEGER, query TEXT)");
        for(auto i : data) {
            output.exec(QString("INSERT INTO fixation(fixation_id,fixation_run_id,fixation_start_event_time,fixation_order_number,x,y,fixation_target,source_file_line,source_file_col,token,syntactic_category,xpath,left_pupil_diameter,right_pupil_diameter,duration,query) VALUES(\"%1\",%2,%3,%4,%5,%6,\"%7\",%8,%9,%10,%11,%12,%13,%14,%15,\"%16\")").arg(i[0]).arg(i[1]).arg(i[2]).arg(i[3]).arg(i[4]).arg(i[5]).arg(i[6]).arg(i[7]).arg(i[8]).arg(i[9] == "null" ? "null" : "\""+i[9]+"\"").arg(i[10] == "null" ? "null" : "\""+i[10]+"\"").arg(i[11] == "null" ? "null" : "\""+i[11]+"\"").arg(i[12]).arg(i[13]).arg(i[14]).arg(safeQuery).replace("\"\"","\""));
        }
        output.close();
        QSqlDatabase::removeDatabase("output");
    }
    /////// TAB SEPARATED VALUES
    else if(output_type == ".tsv") {
        std::ofstream output(savename.toUtf8().constData());
        output << "fixation_id\tfixation_run_id\tfixation_start_event_time\tfixation_order_number\tx\ty\tfixation_target\tsource_file_line\tsource_file_col\ttoken\tsyntactic_category\txpath\tleft_pupil_diameter\tright_pupil_diameter\tduration\n";
        for(auto fix : data) { for(auto value : fix) { output << value.toUtf8().constData() << "\t"; } output << "\n"; }
        output.close();
    }
    /////// XML
    else if(output_type == ".xml") {
        std::ofstream output(savename.toUtf8().constData());
        output << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<itrace_fixation_query query=\"" << safeQuery << "\">\n";
        for(auto fix : data) {
            output << "\t" << (QString("<fixation fixation_id=\"%1\" fixation_run_id=\"%2\" fixation_start_event_time=\"%3\" fixation_order_number=\"%4\" x=\"%5\" y=\"%6\" fixation_target=\"%7\" source_file_line=\"%8\" source_file_col=\"%9\" token=\"%10\" syntactic_category=\"%11\" xpath=\"%12\" left_pupil_diameter=\"%13\" right_pupil_diameter=\"%14\" duration=\"%15\" />").arg(fix[0]).arg(fix[1]).arg(fix[2]).arg(fix[3]).arg(fix[4]).arg(fix[5]).arg(fix[6]).arg(fix[7]).arg(fix[8]).arg(fix[9]).arg(fix[10]).arg(fix[11]).arg(fix[12]).arg(fix[13]).arg(fix[14])).toUtf8().constData() << "\n";
        }
        output << "</itrace_fixation_query>";
        output.close();
    }
    /////// JSON
    else if(output_type == ".json") {
        std::ofstream output(savename.toUtf8().constData());
        output << "{\n\t\"query\": \"" << safeQuery << "\",\n";
        output << "\t\"fixations\": [\n";
        for(auto fix : data) {
            output << (QString("\t{\n\t\t\"fixation_id\": \"%1\",\n\t\t\"fixation_run_id\": \"%2\",\n\t\t\"fixation_start_event_time\": %3,\n\t\t\"fixation_order_number\": %4,\n\t\t\"x\": %5,\n\t\t\"y\": %6,\n\t\t\"fixation_target\": \"%7\",\n\t\t\"source_file_line\": %8,\n\t\t\"source_file_col\": %9,\n\t\t\"token\": \"%10\",\n\t\t\"syntactic_category\": \"%11\",\n\t\t\"xpath\": \"%12\",\n\t\t\"left_pupil_diameter\": %13,\n\t\t\"right_pupil_diameter\": %14,\n\t\t\"duration\": %15\n\t}\n").arg(fix[0]).arg(fix[1]).arg(fix[2]).arg(fix[3]).arg(fix[4]).arg(fix[5]).arg(fix[6]).arg(fix[7]).arg(fix[8]).arg(fix[9]).arg(fix[10]).arg(fix[11]).arg(fix[12]).arg(fix[13]).arg(fix[14]));
        }
        output << "\t]\n}";
    }
    std::cout << output_type << std::endl;
}
