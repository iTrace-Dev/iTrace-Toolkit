#include "controller.h"


/////////////////////////////////////////
// HELPERS
/////////////////////////////////////////

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

void setLineTextToken(QString source_line, int col, QString syntactic_context, QString& token, QString& token_type) {
    // token_type is currently unused?
    QVector<QString> delimiters = {
        "(",")","{","}",".","++","--","+","-",
        "!","~",",","/","%","*","<<",">>",">>>",
        "<","<=",">","<=","==","!=","@","&","^",
        "|","&&","||","?",":","+=","-=","*=",
        "/=","=",";" // Should we add ->?
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

/////////////////////////////////////////
// CLASS METHODS
/////////////////////////////////////////

Controller::Controller(QObject* parent) : QObject(parent) {}

void Controller::saveDatabaseFile(QString file_loc) {
    closeDatabase();
    file_loc.remove("file:///");
    std::ofstream file(file_loc.toUtf8().constData());
    file.close();
    idb = Database(file_loc);

    emit outputToScreen("Successfully created new Database at: "+file_loc);
    emit databaseSet(file_loc);
}

void Controller::loadDatabaseFile(QString file_path) {
    closeDatabase();
    file_path.remove("file:///");
    idb = Database(file_path);

    for(auto i : idb.getSessions()) { emit taskAdded(i); }

    emit outputToScreen("Successfully loaded database.");
    emit databaseSet(file_path);
}

void Controller::closeDatabase() {
    idb.close();
    emit databaseClosed();
}

void Controller::importXMLFile(QString file_path) {

    file_path.remove("file:///");

    if(!idb.isDatabaseOpen()) {
        emit warning("Database Error","There is no Database currently loaded.");
        return;
    }

    //Exit function if file is already imported
    if(idb.fileExists(QCryptographicHash::hash(file_path.toUtf8().constData(),QCryptographicHash::Sha1).toHex())) {
        emit warning("XML Import Error","This XML File has already been imported into the Database.");
        return;
    }

    XMLHandler xml_file(file_path);
    QString type = xml_file.getXMLFileType();

    if(type == "itrace_core") { importCoreXML(file_path); }
    else if(type == "itrace_plugin") { importPluginXML(file_path); }
    else { emit outputToScreen("Unrecognized iTrace file: "+file_path); }
}

void Controller::batchAddXML(QString folder_path) {
    if(!idb.isDatabaseOpen()) {
        emit warning("Database Error","There is no Database currently loaded.");
        return;
    }
    emit setProgressBarToIndeterminate();

    folder_path.remove("file:///");
    std::map<QString,std::pair<std::vector<QString>,bool>> files;
    QDirIterator dir(folder_path);
    while(dir.hasNext()) {
        QString filename = dir.next();
        if(filename.endsWith(".xml")) {
            filename.remove("file:///");
            XMLHandler xml_file(filename);
            QString type = xml_file.getXMLFileType();
            if(type == "itrace_core" || type == "itrace_plugin") {
                QString id = xml_file.getElementAttribute("session_id");
                if(files.count(id) == 0) {
                    auto insert = files.insert(std::make_pair(id,std::make_pair(std::vector<QString>(),false)));
                    insert.first->second.first.push_back(filename);
                }
                else { (files.find(id))->second.first.push_back(filename); }
                if(type == "itrace_core") { files.find(id)->second.second = true; }
            }
        }
        QApplication::processEvents();
    }
    QString badPairWarn, alreadyInWarn;
    for(auto i : files) {
        if(i.second.first.size() >= 2 && i.second.second) {
            for(auto j : i.second.first) {
                if(!idb.fileExists(QCryptographicHash::hash(j.toUtf8().constData(),QCryptographicHash::Sha1).toHex())) { importXMLFile(j); }
                else { alreadyInWarn += "\t" + j + "\n"; }
            }
        }
        else { for(auto j : i.second.first) { badPairWarn += "\t" + j + "\n"; } }
        QApplication::processEvents();
    }

    emit stopProgressBar();

    QString warn;
    if(badPairWarn != "") { warn += "The following files were missing their associated pair file:\n" + badPairWarn; }
    if(alreadyInWarn != "") { warn += "The following files are already imported:\n" + alreadyInWarn; }
    if(warn != "") { emit warning("XML Import Error",warn); }
}

void Controller::importCoreXML(const QString& file_path) {
    QElapsedTimer time;
    time.start();

    XMLHandler core_file(file_path);

    idb.startTransaction();

    QString session_id,
            participant_id,
            task_name,
            session_date_time,
            calibration_id,
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
        QString report = idb.checkAndReturnError();
        if(report != "") { std::cout << "IDB ERROR: " << report.toUtf8().constData() << std::endl; }
        report = core_file.checkAndReturnError();
        if(report != "") { std::cout << "XML ERROR: " << report.toUtf8().constData() << std::endl; }
        QApplication::processEvents();
    }

    idb.commit();

    emit taskAdded(participant_id + " - " + task_name);
    emit outputToScreen(QString("Core file imported. Took %1 seconds").arg(time.elapsed() / 1000.0));

}

void Controller::importPluginXML(const QString& file_path) {
    if(!idb.isDatabaseOpen()) {
        emit warning("Database Error","There is no Database currently loaded.");
        return;
    }

    QElapsedTimer time;
    time.start();

    XMLHandler plugin_file(file_path);

    idb.startTransaction();

    QString session_id,
            ide_plugin_type;

    while(!plugin_file.isAtEnd()) {
        QString element = plugin_file.getNextElementName();

        if(element == "itrace_plugin") {
            session_id = plugin_file.getElementAttribute("session_id");
        }
        else if(element == "environment") {
            std::map<QString,QString> types = {{"MSVS","vstudio_plugin"},{"eclipse","eclipse_plugin"},{"atom","atom_plugin"},{"chrome","chrome_plugin"}};
            ide_plugin_type = plugin_file.getElementAttribute("plugin_type");
            // Insert file
            idb.insertFile(QCryptographicHash::hash(plugin_file.getFilePath().toUtf8().constData(),QCryptographicHash::Sha1).toHex(),session_id,plugin_file.getFilePath(),types.at(ide_plugin_type));
        }
        else if(element == "response") {
            // Insert ide_context
            // The last 4 parameters are unused for the moment
            idb.insertIDEContext(plugin_file.getElementAttribute("event_id"),plugin_file.getElementAttribute("plugin_time"),ide_plugin_type,plugin_file.getElementAttribute("gaze_target"),plugin_file.getElementAttribute("gaze_target_type"),plugin_file.getElementAttribute("source_file_path"),plugin_file.getElementAttribute("source_file_line"),plugin_file.getElementAttribute("source_file_col"),plugin_file.getElementAttribute("editor_line_height"),plugin_file.getElementAttribute("editor_font_height"),plugin_file.getElementAttribute("editor_line_base_x"),plugin_file.getElementAttribute("editor_line_base_y"),"","","","");
        }
        QString report = idb.checkAndReturnError();
        if(report != "") { std::cout << "IDB ERROR: " << report.toUtf8().constData() << std::endl; }
        report = plugin_file.checkAndReturnError();
        if(report != "") { std::cout << "XML ERROR: " << report.toUtf8().constData() << std::endl; }
        QApplication::processEvents();
    }

    idb.commit();

    emit outputToScreen(QString("Plugin file imported. Took %1 seconds").arg(time.elapsed() / 1000.0));
}

void Controller::generateFixationData(QVector<QString> tasks, QString algSettings) {
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

    emit startProgressBar(0,counter-1);

    idb.startTransaction();

    counter = 0;

    for(auto session_id : sessions) {
        //std::cout << "?" << std::endl;
        QVector<Fixation> session_fixations;
        QVector<QString> gaze_targets = idb.getGazeTargetsFromSession(session_id);
        QString fixation_filter_settings;
        for(auto gaze_target : gaze_targets) {
            QVector<Gaze> gazes = idb.getGazesFromSessionAndTarget(session_id,gaze_target);
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
            std::set<long long> unique_gazes;
            for(auto gaze : fix->gaze_vec) {
                if(unique_gazes.find(gaze.event_time) != unique_gazes.end()) { continue; }
                idb.insertFixationGaze(fixation_id,QString::number(gaze.event_time));
            }
        }
        QApplication::processEvents();
    }
    idb.commit();
    emit stopProgressBar();
    emit outputToScreen(QString("Fixation data generated. Elapsed time: %1").arg(time.elapsed() / 1000.0));
}

void Controller::mapTokens(QString srcml_file_path) {
    QElapsedTimer timer;
    timer.start();

    SRCMLHandler srcml(srcml_file_path);

    QVector<QString> all_files = srcml.getAllFilenames();

    idb.startTransaction();

    QVector<std::pair<QString,QString>> files_viewed = idb.getFilesViewed();

    emit startProgressBar(0,files_viewed.size()-1);
    int counter = 0;
    emit outputToScreen("Mapping tokens for "+QString::number(files_viewed.size())+".");

    QString warn = "";
    for(auto file : files_viewed) {
        if(!file.second.isNull() && !file.second.isEmpty()) {
            QString unit_path = findMatchingPath(all_files,file.second);
            if(unit_path == "") {
                warn += "\n" + file.second;
                continue;
            }

            mapSyntax(srcml,unit_path,file.second,true);
            mapToken(srcml,unit_path,file.second,true);
        }
        emit setProgressBarValue(counter); ++counter;
        QApplication::processEvents();
    }

    idb.commit();
    emit stopProgressBar();
    emit outputToScreen(QString("Token Mapping done. Time elasped: %1").arg(timer.elapsed() / 1000.0));
    if(warn != "") {
        warn = "The following gaze targets had no matching unit:" + warn;
        emit warning("Token Mapping Error",warn);
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
        if(i.endsWith(check)) { possible.push_back(i.split("/")); }
    }
    if(possible.size() == 0) { return ""; }

    QString shortest = "";
    int passes = 1;

    QVector<QStringList> candidates;
    while(possible.size() != 1) {
        if(passes > file_split.size()) { return shortest; }
        for(auto unit_path : possible) {
            if(passes > unit_path.size()) {
                if(shortest == "") { shortest = unit_path.join("/"); }
                continue;
            }
            if(unit_path[unit_path.size() - passes] == file_split[file_split.size() - 1]) {
                candidates.push_back(unit_path);
            }
        }
        possible = candidates;
        ++passes;
        QApplication::processEvents();
    }
    if(candidates.size() == 0) { return ""; }
    return candidates[0].join("/");
}

void Controller::mapSyntax(SRCMLHandler& srcml, QString unit_path, QString project_path, bool overwrite) {
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
        if(report != "") { std::cout << "IDB ERROR: " << report.toUtf8().constData() << std::endl; }

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
        cached_gazes.insert(std::make_pair(gaze_key,std::make_pair(xpath,syntactic_context)));
        idb.updateGazeWithSyntacticInfo(response[0],xpath,syntactic_context);
    }
}

void Controller::mapToken(SRCMLHandler& srcml, QString unit_path, QString project_path, bool overwrite) {
    QVector<QVector<QString>> responses = idb.getGazesForSourceMapping(project_path,overwrite);

    QString report = idb.checkAndReturnError();
    if(report != "") { std::cout << "IDB ERROR: " << report.toUtf8().constData() << std::endl; }

    QStringList unit_body = srcml.getUnitBody(unit_path).split("\n");
    //std::cout << "UNIT BODY SIZE: " << unit_body.size() << std::endl;;
    //for(auto i : unit_body) { std::cout << i.toUtf8().constData() << std::endl; }

    std::map<QString,std::pair<QString,QString>> cached_gazes;
    //std::cout << "RESPONSES SIZE TOKEN: " << responses.size() << std::endl;
    for(auto response : responses) {
        int res_line = response[1].toInt() - 1,
            res_col = response[2].toInt() - 1;
        QString token = "",
                token_type = "";

        QString report = idb.checkAndReturnError();
        if(report != "") { std::cout << "IDB ERROR: " << report.toUtf8().constData() << std::endl; }

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



