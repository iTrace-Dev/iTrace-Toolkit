#include "database.h"


Database::Database(QObject* parent) : QObject(parent) {}

void Database::openDatabase(QString filePath) {
    if(filePath == "") { return; }
    if(db.isOpen()) { db.close(); }
    path = filePath;
    filePath.remove("file:///");                         // This might cause problems with cross compatability
    db = QSqlDatabase::addDatabase("QSQLITE");           //
    db.setDatabaseName((filePath.toUtf8().constData())); //
    db.open();
    if(!db.isOpen()) { std::cout << "Can't open DB" << std::endl; }
    else {
        //QSqlQuery query(db);
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
        db.exec("CREATE TABLE IF NOT EXISTS files(file_hash TEXT PRIMARY KEY,file_full_path TEXT)");
        db.commit();
    }
}

void Database::backupDatabase() {
    //TODO - CREATE A BACKUP BEFORE ADDING XML FILES
}

bool Database::addXMLFile(QString filePath) {
    if(!isDatabaseOpen()) { return false; }
    filePath.remove("file:///");
    std::cout << "XML FILE: " << filePath.toUtf8().constData() << std::endl;
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
    bool rtn;

    xml.readNextStartElement();
    if(xml.name() == "itrace_core") {
        rtn = addCoreXMLFile(xml);
    }


    file.close();
    return rtn;
}

bool Database::addCoreXMLFile(QXmlStreamReader& xml) {
    QElapsedTimer time;
    time.start();

    QString session_id = xml.attributes().value("session_id").toUtf8().constData(),
            calibration_id,
            calibration_point_id,
            calibration_x,
            calibration_y,
            participant_id = xml.attributes().value("participant_id").toUtf8().constData();

    bool skipCalibration = false;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //   The QStrings for this section are gonna be long and ugly. Any solution to shortening them will be appreciated
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QString session_time = xml.attributes().value("session_date_time").toUtf8().constData();
    db.exec(QString("INSERT INTO session(session_id,participant_id,session_date,session_time,task_name) VALUES(%1,\"%2\",%3,%4,\"%5\")").arg(session_id).arg(participant_id).arg(session_time).arg(session_time).arg(xml.attributes().value("task_name")));

    if(!participantExists(participant_id)) {
        db.exec(QString("INSERT INTO participant(participent_id,session_length) VALUES(\"%1\",\"\")").arg(participant_id));
    }
    while(!xml.atEnd()) {
        if(xml.readNextStartElement()) {
            QStringRef tag = xml.name();

            QXmlStreamAttributes attr = xml.attributes();

            if(tag == "environment") {
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
                //db.exec("INSERT INTO gaze(event_time,session_id,calibration_id,participant_id,tracker_time,system_time,x,y,left_x,left_y,left_pupil_diameter,left_validation,right_x,right_y,right_pupil_diameter,right_validation,user_left_x,user_left_y,user_left_z,user_right_x,user_right_y,user_right_z) VALUES(" + attr.value("event_id")+","+session_id+","+calibration_id+",\""+participant_id+"\",\""+attr.value("tracker_time")+"\",\""+attr.value("core_time")+"\",\""+attr.value("x")+"\",\""+attr.value("y")+"\",\""+attr.value("left_x")+"\",\""+attr.value("left_y")+"\",\""+attr.value("left_pupil_diameter")+"\",\""+attr.value("left_validation")+"\",\""+attr.value("right_x")+"\",\""+attr.value("right_y")+"\",\""+attr.value("right_pupil_diameter")+"\",\""+attr.value("right_validation")+"\",\""+attr.value("user_left_x")+"\",\""+attr.value("user_left_y")+"\",\""+attr.value("user_left_z")+"\",\""+attr.value("user_right_x")+"\",\""+attr.value("user_right_y")+"\",\""+attr.value("user_right_z")+"\")");
            }
            else { std::cout << "UNRECONGIZED TAG NAME: " << tag.toUtf8().constData() << std::endl; }
            QString report =  db.lastError().text();
            if(report != "") { std::cout << "ERROR:" << report.toUtf8().constData() << std::endl; }
        }
    }

    if(xml.hasError()) {
        std::cout << "An error occured" << std::endl;
        std::cout << xml.error() << std::endl;
        std::cout << xml.errorString().toUtf8().constData() << std::endl;
    }

    std::cout << calibrationExists("1557435760934") << std::endl;

    db.commit();

    std::cout << "ELAPSED TIME: " << time.elapsed() << std::endl;

    return true;

}

bool Database::isDatabaseOpen() {
    return db.isOpen();
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
