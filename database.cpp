#include "database.h"


//TODO
// - This DESPERATELY needs to be broken up into smaller classes.
//   Database has access to way too much
// - Make error report function


Database::Database(QObject* parent) : QObject(parent) {}

void Database::createNewDatabase() {
    QString fileName = QFileDialog::getSaveFileName(nullptr,"Save as","./","SQLite Files (*.db3;*.db;*.sqlite;*.sqlite3);;All Files (*.*)");
    if(fileName == "") { return; }
    std::ofstream file(fileName.toUtf8().constData());
    file.close();
    if(db.isOpen()) { db.close(); }
    path = fileName;
    fileName.remove("file:///");                         // This might cause problems with cross compatability
    db = QSqlDatabase::addDatabase("QSQLITE");           //
    db.setDatabaseName((fileName.toUtf8().constData())); //
    db.open();
    if(!db.isOpen()) { std::cout << "Can't open DB" << std::endl; return; }
    else {
        // Create the database if it already doesn't exist
        db.exec("CREATE TABLE IF NOT EXISTS participant(participant_id TEXT PRIMARY KEY,session_length INTEGER)");
        db.exec("CREATE TABLE IF NOT EXISTS fixation_run(fixation_run_id INTEGER PRIMARY KEY,session_id INTEGER,date_time INTEGER,filter TEXT,FOREIGN KEY (session_id) REFERENCES session(session_id))");
        db.exec("CREATE TABLE IF NOT EXISTS session(session_id INTEGER PRIMARY KEY,participant_id TEXT,screen_width INTEGER, screen_height INTEGER,tracker_type TEXT, tracker_serial_number TEXT,session_date INTEGER, session_time INTEGER,screen_recording_start INTEGER,task_name TEXT,FOREIGN KEY (participant_id) REFERENCES participant(participant_id))");
        db.exec("CREATE TABLE IF NOT EXISTS fixation(fixation_id TEXT PRIMARY KEY,fixation_run_id INTEGER,fixation_start_event_time INTEGER,fixation_order_number INTEGER,x INTEGER,y INTEGER,fixation_target TEXT,source_file_line INTEGER, source_file_col INTEGER,token TEXT,syntactic_category TEXT,xpath TEXT,left_pupil_diameter REAL,right_pupil_diameter REAL,duration INTEGER)");
        db.exec("CREATE TABLE IF NOT EXISTS calibration(calibration_id INTEGER PRIMARY KEY)");
        db.exec("CREATE TABLE IF NOT EXISTS calibration_point(calibration_point_id TEXT,calibration_id INTEGER,calibration_x REAL,calibration_y REAL,FOREIGN KEY (calibration_id) REFERENCES calibration(calibration_id))");
        db.exec("CREATE TABLE IF NOT EXISTS calibration_sample(calibration_point_id TEXT,left_x REAL, left_y REAL,left_validation REAL,right_x REAL, right_y REAL,right_validation REAL,FOREIGN KEY (calibration_point_id) REFERENCES calibration_point(calibration_point_id))");
        db.exec("CREATE TABLE IF NOT EXISTS gaze(event_time INTEGER PRIMARY KEY,session_id INTEGER,calibration_id INTEGER,participant_id TEXT, tracker_time INTEGER, system_time INTEGER, x REAL, y REAL,left_x REAL, left_y REAL, left_pupil_diameter REAL, left_validation INTEGER,right_x REAL, right_y REAL, right_pupil_diameter REAL, right_validation INTEGER,user_left_x REAL,user_left_y REAL,user_left_z REAL,user_right_x REAL,user_right_y REAL,user_right_z REAL,FOREIGN KEY (session_id) REFERENCES session(session_id),FOREIGN KEY (calibration_id) REFERENCES calibration(calibration_id),FOREIGN KEY (participant_id) REFERENCES participant(participant_id))");
        db.exec("CREATE TABLE IF NOT EXISTS ide_context(event_time INTEGER,time_stamp TEXT,ide_type TEXT,gaze_target TEXT,gaze_target_type TEXT,source_file_path TEXT, source_file_line INTEGER, source_file_col INTEGER,editor_line_height REAL,editor_font_height REAL, editor_line_base_x REAL, editor_line_base_y REAL,source_token TEXT,source_token_type TEXT, source_token_xpath TEXT, source_token_syntactic_context TEXT,FOREIGN KEY (event_time) REFERENCES gaze(event_time))");
        db.exec("CREATE TABLE IF NOT EXISTS web_context(event_time INTEGER,browser_type TEXT,site_name TEXT,url TEXT,tag TEXT,FOREIGN KEY (event_time) REFERENCES gaze(event_time))");
        db.exec("CREATE TABLE IF NOT EXISTS fixation_gaze(fixation_id INTEGER,event_time INTEGER,FOREIGN KEY (fixation_id) REFERENCES fixation(fixation_id),FOREIGN KEY (event_time) REFERENCES gazes(event_time))");
        db.exec("CREATE TABLE IF NOT EXISTS files(file_hash TEXT PRIMARY KEY,session_id INTEGER,file_full_path TEXT,file_type TEXT,FOREIGN KEY (session_id) REFERENCES session(session_id))");
        db.commit();
    }
    emit outputToScreen("New Database file created: " + fileName);
}

void Database::openDatabase() {
    //TODO
    // Check if provided database isn't an iTrace database, and reject if it isn't

    QString fileName = QFileDialog::getOpenFileName(nullptr,"Open Database file","./","SQLite Files (*.db3;*.db;*.sqlite;*.sqlite3);;All Files (*.*)");
    if(fileName == "") { return; }

    path = fileName;
    fileName.remove("file:///");                         // This might cause problems with cross compatability
    db = QSqlDatabase::addDatabase("QSQLITE");           //
    db.setDatabaseName((fileName.toUtf8().constData())); //
    db.open();
    if(!db.isOpen()) { std::cout << "Can't open DB" << std::endl; return; }

    QSqlQuery sessions = db.exec("SELECT session_id FROM session");
    while(sessions.next()) {
        QString session_id = sessions.value(0).toString();
        QSqlQuery task_info = db.exec(QString("SELECT participant_id, task_name from session WHERE session_id = %1").arg(session_id));
        task_info.first();
        emit taskAdded(task_info.value(0).toString() + " - " + (task_info.value(1).toString()));
    }

    emit outputToScreen("Database file loaded: " + fileName);
}

void Database::importXML() {
    QString fileName = QFileDialog::getOpenFileName(nullptr,"Import iTrace XML","./","SrcML Files (*.xml;*.srcml;);;All Files (*.*)");
    if(fileName == "") { return; }
    addXMLFile(fileName);
}

void Database::backupDatabase() {
    //TODO - CREATE A BACKUP BEFORE ADDING XML FILES
}

bool Database::addXMLFile(QString filePath) {
    if(!isDatabaseOpen()) { return false; }
    filePath.remove("file:///");

    QFile file(filePath);
    if(!file.open(QFile::ReadOnly | QFile::Text)) { std::cout << "Cannot read file" << file.errorString().constData() << std::endl; }

    QXmlStreamReader xml(&file);


    /*
    To get name of element:
        xml.name()
    To get attribute of an element:
        xml.attributes().value("valueName").toSring()
    To get element's text
        xml.readElementText()
    */
    bool rtn = false;

    xml.readNextStartElement();
    file.close();
    if(xml.name() == "itrace_core") { // Core File
        rtn = addCoreXMLFile(filePath);
    }
    else if(xml.name() == "itrace_plugin") { // Plugin File
        rtn = addPluginXMLFile(filePath);
    }
    else { // Unregognized File
         emit outputToScreen("NOT AN iTRACE XML FILE: " + filePath);
    }

    return rtn;
}

void Database::batchAddXMLFiles() {
    if(!isDatabaseOpen()) { return; }

    QString dir = QFileDialog::getExistingDirectory(nullptr, "Select folder","");

    emit outputToScreen("Batch importing folder: " + dir);

    dir.remove("file:///");
    QDirIterator dirItr(dir);


    //TODO
    // Multiple plugins can be associated with 1 core - ensure 1 core and at LEAST 1 plugin are grouped together
    //
    // Find cores first, then check the plugins
    std::map<QString,std::vector<QString>> files;
    while(dirItr.hasNext()) {
        QString fileName = dirItr.next().toUtf8().constData();
        if(fileName.endsWith(".xml")) {
            fileName.remove("file:///");
            QFile file(fileName);
            file.open(QFile::ReadOnly | QFile::Text);
            QXmlStreamReader xml(&file);
            xml.readNextStartElement();
            file.close();
            if(xml.name() == "itrace_core" || xml.name() == "itrace_plugin") {
                QString id = xml.attributes().value("session_id").toUtf8().constData();
                if(files.count(id) == 0) {
                    auto insert = files.insert(std::make_pair(id,std::vector<QString>()));
                    insert.first->second.push_back(fileName);
                }
                else {
                    auto insert = files.find(id);
                    insert->second.push_back(fileName);
                }
            }
        }
    }
    QString warning;
    for(auto i : files) {
        if(i.second.size() >= 2) { for (auto j : i.second) { addXMLFile(j); } }
        else { warning += i.second[0] + "\n"; }
    }
    if(warning != "") {
        emit outputToScreen("The following file(s) had no pair:\n"+warning);
        QApplication::processEvents();
    }

}

bool Database::addCoreXMLFile(const QString& filePath) {
    QElapsedTimer time;
    time.start();

    // Check if file is already in database
    QString filehash = QCryptographicHash::hash(filePath.toUtf8().constData(),QCryptographicHash::Sha1).toHex();
    if(fileExists(filehash)) {
        return false;
    }

    QFile file(filePath);
    if(!file.open(QFile::ReadOnly | QFile::Text)) { std::cout << "Cannot read file" << file.errorString().constData() << std::endl; }
    QXmlStreamReader xml(&file);

    QString session_id,
            calibration_id,
            calibration_point_id,
            calibration_x,
            calibration_y,
            participant_id,
            task_id;

    bool skipCalibration = false;

    db.exec("BEGIN"); // Begins a transaction to speed up the process of inserting.

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //   The QStrings for this section are gonna be long and ugly. Any solution to shortening them will be appreciated
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    while(!xml.atEnd()) {
        if(xml.readNextStartElement()) {
            QStringRef tag = xml.name();
            QXmlStreamAttributes attr = xml.attributes();

            if(tag == "itrace_core") {
                session_id = xml.attributes().value("session_id").toUtf8().constData();
                participant_id = xml.attributes().value("participant_id").toUtf8().constData();
                task_id = xml.attributes().value("task_name").toUtf8().constData();
                QString session_time = xml.attributes().value("session_date_time").toUtf8().constData();
                db.exec(QString("INSERT INTO session(session_id,participant_id,session_date,session_time,task_name) VALUES(%1,\"%2\",%3,%4,\"%5\")").arg(session_id).arg(participant_id).arg(session_time).arg(session_time).arg(task_id));
                if(!participantExists(participant_id)) {
                    db.exec(QString("INSERT INTO participant(participant_id,session_length) VALUES(\"%1\",null)").arg(participant_id));
                    QString report =  db.lastError().text();
                    if(report != "") { std::cout << "ERROR:" << report.toUtf8().constData() << std::endl; }
                }
                db.exec(QString("INSERT INTO files(file_hash,session_id,file_full_path,file_type) VALUES(\"%1\",%2,\"%3\",\"core\")").arg(filehash).arg(session_id).arg(filePath));
            }
            else if(tag == "environment") {
                db.exec(QString("UPDATE session SET screen_width = %1,screen_height = %2,tracker_type = \"%3\",tracker_serial_number = \"%4\",screen_recording_start = %5 WHERE session_id = %6").arg(attr.value("screen_width")).arg(attr.value("screen_height")).arg(attr.value("tracker_type")).arg(attr.value("tracker_serial_number")).arg(attr.value("screen_recording_start")).arg(session_id));
            }
            else if(tag == "calibration") {
                calibration_id = attr.value("timestamp").toUtf8().constData();
                if(!calibrationExists(calibration_id)) {
                    db.exec(QString("INSERT INTO calibration(calibration_id) VALUES(%1)").arg(calibration_id));
                    skipCalibration = false;
                }
                else { skipCalibration = true; }
            }
            else if(tag == "calibration_point" && !skipCalibration) { // These ternary operators are to reformat 0 as 0.0 to maintain the same SHA1 hashes as previous programs
                calibration_x = (QString("%1").arg(attr.value("x")) == "0") ? "0.0" : QString("%1").arg(attr.value("x"));
                calibration_y = (QString("%1").arg(attr.value("y")) == "0") ? "0.0" : QString("%1").arg(attr.value("y"));
                calibration_point_id = QCryptographicHash::hash((calibration_id+calibration_x+calibration_y).toUtf8().constData(),QCryptographicHash::Sha1).toHex();
                db.exec(QString("INSERT INTO calibration_point(calibration_point_id,calibration_id,calibration_x,calibration_y) VALUES(\"%1\",%2,%3,%4)").arg(calibration_point_id).arg(calibration_id).arg(calibration_x).arg(calibration_y));
            }
            else if(tag == "sample" && !skipCalibration) {
                db.exec(QString("INSERT INTO calibration_sample(calibration_point_id,left_x,left_y,left_validation,right_x,right_y,right_validation) VALUES(\"%1\",%2,%3,%4,%5,%6,%7)").arg(calibration_point_id).arg(attr.value("left_x")).arg(attr.value("left_y")).arg(attr.value("left_validity")).arg(attr.value("right_x")).arg(attr.value("right_y")).arg(attr.value("right_validity")));
            }
            else if(tag == "response") {
                db.exec(QString("INSERT INTO gaze(event_time,session_id,calibration_id,participant_id,tracker_time,system_time,x,y,left_x,left_y,left_pupil_diameter,left_validation,right_x,right_y,right_pupil_diameter,right_validation,user_left_x,user_left_y,user_left_z,user_right_x,user_right_y,user_right_z) VALUES(%1,%2,%3,\"%4\",\"%5\",\"%6\",\"%7\",\"%8\",\"%9\",\"%10\",\"%11\",\"%12\",\"%13\",\"%14\",\"%15\",\"%16\",\"%17\",\"%18\",\"%19\",\"%20\",\"%21\",\"%22\")").arg(attr.value("event_id")).arg(session_id).arg(calibration_id).arg(participant_id).arg(attr.value("tracker_time")).arg(attr.value("core_time")).arg(attr.value("x")).arg(attr.value("y")).arg(attr.value("left_x")).arg(attr.value("left_y")).arg(attr.value("left_pupil_diameter")).arg(attr.value("left_validation")).arg(attr.value("right_x")).arg(attr.value("right_y")).arg(attr.value("right_pupil_diameter")).arg(attr.value("right_validation")).arg(attr.value("user_left_x")).arg(attr.value("user_left_y")).arg(attr.value("user_left_z")).arg(attr.value("user_right_x")).arg(attr.value("user_right_y")).arg(attr.value("user_right_z")));
            }
            else { std::cout << "UNRECONGIZED TAG NAME: " << tag.toUtf8().constData() << std::endl; }
            QString report =  db.lastError().text();
            if(report != "") { std::cout << "ERROR:" << report.toUtf8().constData() << std::endl; }
            QApplication::processEvents();
        }
    }

    if(xml.hasError()) {
        std::cout << "An error occured" << std::endl;
        std::cout << xml.error() << std::endl;
        std::cout << xml.errorString().toUtf8().constData() << std::endl;
    }

    db.exec("COMMIT");
    file.close();

    // Adding Participant to the participant list
    emit taskAdded(participant_id + " - " + task_id);

    emit outputToScreen("CORE FILE IMPORTED: " + filePath);
    emit outputToScreen(QString("ELAPSED TIME in ms: ") + QString::number(time.elapsed(),10));
    QApplication::processEvents();

    return true;
}

bool Database::addPluginXMLFile(const QString& filePath) {
    //TODO
    // For database, store the type of pluign under filetype - i.e., "chrome-plugin", "msvs-plugin"

    QElapsedTimer time;
    time.start();

    // Check if file is already in database
    QString filehash = QCryptographicHash::hash(filePath.toUtf8().constData(),QCryptographicHash::Sha1).toHex();
    if(fileExists(filehash)) {
        return false;
    }

    QFile file(filePath);
    if(!file.open(QFile::ReadOnly | QFile::Text)) { std::cout << "Cannot read file" << file.errorString().constData() << std::endl; }
    QXmlStreamReader xml(&file);

    QString ide_plugin_type;

    db.exec("BEGIN");

    while(!xml.atEnd()) {
        if(xml.readNextStartElement()) {
            QStringRef tag = xml.name();
            QXmlStreamAttributes attr = xml.attributes();

            if(tag == "itrace_plugin") {
                db.exec(QString("INSERT INTO files(file_hash,session_id,file_full_path,file_type) VALUES(\"%1\",%2,\"%3\",\"plugin\")").arg(filehash).arg(attr.value("session_id")).arg(filePath));
            }
            else if(tag == "environment") {
                ide_plugin_type = attr.value("plugin_type").toUtf8().constData();
            }
            else if(tag == "response") {
                db.exec(QString("INSERT INTO ide_context(event_time,time_stamp,ide_type,gaze_target,gaze_target_type,source_file_path,source_file_line,source_file_col,editor_line_height,editor_font_height,editor_line_base_x,editor_line_base_y) VALUES(%1,\"%2\",\"%3\",\"%4\",\"%5\",\"%6\",%7,%8,%9,%10,%11,%12)").arg(attr.value("event_id")).arg(attr.value("plugin_time")).arg(ide_plugin_type).arg(attr.value("gaze_target")).arg(attr.value("gaze_target_type")).arg(attr.value("source_file_path")).arg(attr.value("source_file_line")).arg(attr.value("source_file_col")).arg((attr.value("editor_line_height") == "") ? "null" : attr.value("editor_line_height").toUtf8().constData()).arg((attr.value("editor_font_height") == "") ? "null" : attr.value("editor_font_height").toUtf8().constData()).arg((attr.value("editor_line_base_x") == "") ? "null" : attr.value("editor_line_base_x").toUtf8().constData()).arg((attr.value("editor_line_base_y") == "") ? "null" : attr.value("editor_line_base_y").toUtf8().constData()));
            }
            else { std::cout << "UNRECONGIZED TAG NAME: " << tag.toUtf8().constData() << std::endl; }
            QString report =  db.lastError().text();
            if(report != "") { std::cout << "ERROR:" << report.toUtf8().constData() << std::endl; }
            QApplication::processEvents();
        }
    }

    if(xml.hasError()) {
        std::cout << "An error occured" << std::endl;
        std::cout << xml.error() << std::endl;
        std::cout << xml.errorString().toUtf8().constData() << std::endl;
    }

    db.exec("COMMIT");
    file.close();

    emit outputToScreen("PLUGIN FILE IMPORTED: " + filePath);
    emit outputToScreen(QString("ELAPSED TIME in ms: ") + QString::number(time.elapsed(),10));
    QApplication::processEvents();

    return true;
}

void Database::generateFixations(QVector<QString> tasks) {
    // TODO
    // - This function is massive and will be even more massive as new algorithms are added.
    //      Move to a different file; maybe
    // - Add comments explainging the algorithms
    // - Measure time that elapses and display to output log

    // HARDCODED VALUES - will change later as popup window is made
    QString algorithm = "BASIC";
    int window_size = 4, radius = 35, peak = 40;

    std::vector<QString> sessions;
    for(auto task : tasks) {
        QStringList values = task.split(" - ");
        if(values[2] == "1") {
            QSqlQuery id = db.exec(QString("SELECT session_id FROM session WHERE participant_id = \"%1\" AND task_name = \"%2\"").arg(values[0]).arg(values[1]));
            id.first();
            sessions.push_back(id.value(0).toString());
        }
    }

    db.exec("BEGIN");
    for(auto session_id : sessions) {
        std::vector<Fixation> session_fixations;
        QSqlQuery gazes = db.exec(QString("SELECT DISTINCT ide_context.gaze_target FROM ide_context JOIN gaze ON gaze.event_time=ide_context.event_time WHERE ide_context.gaze_target != \"\" AND gaze.session_id = %1").arg(session_id));

        while(gazes.next()) {
            QString gaze_target = gazes.value(0).toString();
            QSqlQuery session_gazes = db.exec(QString("SELECT gaze.event_time, gaze.x, gaze.y, gaze.system_time, gaze.left_pupil_diameter, gaze.right_pupil_diameter, gaze.left_validation, gaze.right_validation, ide_context.gaze_target, ide_context.gaze_target_type, ide_context.source_file_line, ide_context.source_file_col, ide_context.source_token, ide_context.source_token_xpath, ide_context.source_token_syntactic_context FROM gaze JOIN ide_context ON gaze.event_time=ide_context.event_time WHERE gaze.session_id = %1 AND ide_context.gaze_target = \"%2\" ORDER BY gaze.event_time ASC").arg(session_id).arg(gaze_target));

            if(algorithm == "BASIC") {
            //buildRawGazeVector - olsson step 1
                // First, create vector of raw Gazes
                std::vector<Gaze> raw;
                // Keep track of last known valid
                Gaze last_valid = Gaze();
                while(session_gazes.next()) {
                    Gaze data(session_gazes);
                    if(data.isValid()) {
                        raw.push_back(data);
                        last_valid = data;
                    }
                    else { if(last_valid.isValid()) { raw.push_back(last_valid); } }
                }

            //calculateDifferenceVector - olsson step 2
                std::vector<double> differences;
                for(int i = window_size; i < int(raw.size()) + 1 - window_size; ++i) {
                    std::pair<double,double> before = {0.0,0.0},
                                           after =  {0.0,0.0};
                    for(int j = 0; j < window_size; ++j) {
                        before.first += raw[i - (j + 1)].x;
                        before.second += raw[i - (j + 1)].y;
                        after.first += raw[i + j].x;
                        after.second += raw[i + j].y;
                    }
                    before.first /= window_size;
                    before.second /= window_size;
                    after.first /= window_size;
                    after.second /= window_size;
                    differences.push_back(sqrt(pow(after.first - before.first,2) + pow(after.second - before.second,2)));
                }

            //calculatePeakIndices - olsson step 3-5
                //step 3
                std::vector<double> peaks(differences.size());
                for(int i = 0; i < int(differences.size()); ++i) { peaks[i] = 0.0; }
                for(int i = 1; i < int(differences.size()) - 1; ++i) {
                    if(differences[i] > differences[i-1] && differences[i] > differences[i+1]) {
                        peaks[i] = differences[i];
                    }
                }

                //step 4
                for(int i = window_size-1; i < int(peaks.size()); ++i) {
                    int start = i - (window_size - 1),
                        end = i;
                    while(start != end) {
                        if(peaks[start] >= peaks[end]) {
                            peaks[end] = 0.0;
                            --end;
                        }
                        else {
                            peaks[start] = 0.0;
                            ++start;
                        }
                    }
                }
                //step 5
                std::vector<int> indicies;
                for(int i = 0; i < int(peaks.size()); ++i) {
                    if(peaks[i] >= peak) {
                        indicies.push_back(i);
                    }
                }

             //calculateSpaitialFixations
                double shortest_dis = 0;
                std::vector<Fixation> fixations;

                while(shortest_dis < radius) {

                    fixations.clear();
                    int start_peak_index = 0;

                    for(auto index : indicies) {
                        std::vector<Gaze> slice;
                        auto start = raw.begin() + start_peak_index;
                        auto end = raw.begin() + index;
                        copy(start, end, std::back_inserter(slice));
                        //computerFixationEstimate
                        Fixation fix = Fixation();
                        fix.computeFixationEstimate(slice);
                        //end computerFixationEstimate
                        fixations.push_back(fix);
                        start_peak_index = index;
                    }
                    Fixation fix = Fixation();
                    std::vector<Gaze> slice;
                    copy(raw.begin() + start_peak_index, raw.end(), std::back_inserter(slice));
                    fix.computeFixationEstimate(slice);
                    fixations.push_back(fix);

                    shortest_dis = INFINITY;
                    Fixation* previous_estimate = nullptr;
                    int peak_index = -1, peak_removal_index = -1;
                    auto crnt = fixations.begin();
                    for(; crnt != fixations.end(); ++crnt) {
                        if(previous_estimate != nullptr) {
                            double distance = sqrt(pow((*crnt).x - (*previous_estimate).x,2) + pow((*crnt).y - (*previous_estimate).y,2));
                            if(distance < shortest_dis) {
                                shortest_dis = distance;
                                peak_removal_index = peak_index;
                            }
                        }
                        previous_estimate = &*crnt;
                        ++peak_removal_index;
                    }
                    if(shortest_dis < radius) { indicies.erase(indicies.begin() + peak_removal_index); }
                }

                //fixations are done
                for(auto i : fixations) { session_fixations.push_back(i); }
            }
        }

        for(auto item = session_fixations.begin(); item != session_fixations.end(); ++item) {
            item->calculateDatabaseFields();
        }

        std::sort(session_fixations.begin(), session_fixations.end(), [](const Fixation& a, const Fixation& b) -> bool { return a.fixation_event_time > b.fixation_event_time; });

        QString fixation_filter_settings = algorithm + "," + QString::number(window_size) + "," + QString::number(radius) + "," + QString::number(peak);

        //Insert into database
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        long long fixation_run_id = ms.count();
        long long fixation_date_time = fixation_run_id; // This will probably be changed in the future
        db.exec(QString("INSERT INTO fixation_run (fixation_run_id, session_id, date_time, filter) VALUES(%1,%2,%3,\"%4\")").arg(fixation_run_id).arg(session_id).arg(fixation_date_time).arg(fixation_filter_settings));
        QString report =  db.lastError().text();
        if(report != "") { std::cout << "ERROR:" << report.toUtf8().constData() << std::endl; }
        int fixation_order = 1;

        for(auto fix = session_fixations.begin(); fix != session_fixations.end(); ++fix) {
            QString fixation_id = QUuid::createUuid().toString();
            fixation_id.remove("{"); fixation_id.remove("}");
            db.exec(QString("INSERT INTO fixation (fixation_id,fixation_run_id,fixation_start_event_time,fixation_order_number,x,y,fixation_target,source_file_line,source_file_col,token,syntactic_category,xpath,left_pupil_diameter,right_pupil_diameter,duration) VALUES(\"%1\",%2,%3,%4,%5,%6,\"%7\",%8,%9,%10,%11,%12,%13,%14,%15)").arg(fixation_id).arg(fixation_run_id).arg(fix->fixation_event_time).arg(fixation_order).arg(fix->x).arg(fix->y).arg(fix->target).arg(fix->source_file_line).arg(fix->source_file_col).arg(fix->token == "" ? "null" : "\""+fix->token+"\"").arg(fix->syntactic_category == "" ? "null" : "\""+fix->syntactic_category+"\"").arg(fix->xpath == "" ? "null" : "\""+fix->xpath+"\"").arg(fix->left_pupil_diameter).arg(fix->right_pupil_diameter).arg(fix->duration));
            QString report =  db.lastError().text();
            if(report != "") { std::cout << "ERROR:" << report.toUtf8().constData() << std::endl; }
            ++fixation_order;
            std::set<long long> unique_gazes;
            for(auto gaze : fix->gaze_vec) {
                if(unique_gazes.find(gaze.event_time) != unique_gazes.end()) { continue; }
                db.exec(QString("INSERT INTO fixation_gaze (fixation_id, event_time) VALUES (\"%1\",%2)").arg(fixation_id).arg(gaze.event_time));
                QString report =  db.lastError().text();
                if(report != "") { std::cout << "ERROR:" << report.toUtf8().constData() << std::endl; }
            }
        }
    }
    db.exec("COMMIT");
    emit outputToScreen("Fixation Data Generated");
}

bool Database::isDatabaseOpen() {
    return db.isOpen();
}

// Maybe consolidate these into one function? Where the arguments are function parameters
    // Issues with that: how to know whether selecting text or number data?

bool Database::fileExists(const QString& hash) {
    QSqlQuery qry = db.exec(QString("SELECT file_hash FROM files WHERE file_hash = \"%1\"").arg(hash));
    qry.last();
    return qry.at() + 1 > 0;
}

bool Database::participantExists(const QString& id) {
    QSqlQuery qry = db.exec(QString("SELECT participant_id FROM participant WHERE participant_id = \"%1\"").arg(id));
    qry.last();
    return qry.at() + 1 > 0;
}

bool Database::calibrationExists(const QString& id) {
    QSqlQuery qry = db.exec(QString("SELECT calibration_id FROM calibration WHERE calibration_id = %1").arg(id));
    qry.last();
    return qry.at() + 1 > 0;
}
