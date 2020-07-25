#include "controller.h"

Controller::Controller(QObject* parent) : QObject(parent) {}

void Controller::saveDatabaseFile(QString file_loc) {
    file_loc.remove("file:///");
    std::ofstream file(file_loc.toUtf8().constData());
    file.close();
    idb = Database(file_loc);
    emit outputToScreen("Successfully created new Database at: "+file_loc);
}

void Controller::loadDatabaseFile(QString file_path) {
    file_path.remove("file:///");
    idb = Database(file_path);

    for(auto i : idb.getSessions()) { emit taskAdded(i); }
    emit outputToScreen("Successfully loaded database.");
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
    else { /* Emit warning here */ }
}

void Controller::batchAddXML(QString folder_path) {
    if(!idb.isDatabaseOpen()) {
        emit warning("Database Error","There is no Database currently loaded.");
        return;
    }

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
                //xml_file.getNextElementName();
                QString id = xml_file.getElementAttribute("session_id");
                std::cout << xml_file.checkAndReturnError().toUtf8().constData() << std::endl;
                std::cout << "ID: " << id.toUtf8().constData() << std::endl;
                if(files.count(id) == 0) {
                    auto insert = files.insert(std::make_pair(id,std::make_pair(std::vector<QString>(),false)));
                    insert.first->second.first.push_back(filename);
                }
                else { (files.find(id))->second.first.push_back(filename); }
                if(type == "itrace_core") { files.find(id)->second.second = true; }
            }
        }
    }
    QString badPairWarn, alreadyInWarn;
    for(auto i : files) {
        std::cout << "Num: " << i.second.first.size() << std::endl;
        if(i.second.first.size() >= 2 && i.second.second) {
            for(auto j : i.second.first) {
                if(!idb.fileExists(QCryptographicHash::hash(j.toUtf8().constData(),QCryptographicHash::Sha1).toHex())) { importXMLFile(j); }
                else { alreadyInWarn += "\t" + j + "\n"; }
            }
        }
        else { for(auto j : i.second.first) { badPairWarn += "\t" + j + "\n"; } }
    }

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
    }

    idb.commit();

    emit taskAdded(participant_id + " - " + task_name);
    emit outputToScreen(QString("Core file imported. Took %1 seconds").arg(time.elapsed() / 1000));

}

void Controller::importPluginXML(const QString& file_path) {
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
    }

    idb.commit();

    emit outputToScreen(QString("Plugin file imported. Took %1 seconds").arg(time.elapsed() / 1000));
}

void Controller::generateFixationData(QVector<QString> tasks, QString type) {
    // TODO
}
