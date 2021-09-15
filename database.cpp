#include "database.h"

Database::Database() {
    db = QSqlDatabase::addDatabase("QSQLITE");
}

Database::Database(QString fileURL) : Database() {
    file_path = fileURL;
    file_path.remove("file:///");

    db.setDatabaseName(file_path.toUtf8().constData());
    db.open();

    //Create tables here
    db.exec("CREATE TABLE IF NOT EXISTS participant(participant_id TEXT PRIMARY KEY,session_length INTEGER)");
    db.exec("CREATE TABLE IF NOT EXISTS fixation_run(fixation_run_id INTEGER PRIMARY KEY,session_id INTEGER,date_time INTEGER,filter TEXT,FOREIGN KEY (session_id) REFERENCES session(session_id))");
    db.exec("CREATE TABLE IF NOT EXISTS session(session_id INTEGER PRIMARY KEY,participant_id TEXT,screen_width INTEGER, screen_height INTEGER,tracker_type TEXT, tracker_serial_number TEXT,session_date INTEGER, session_time INTEGER,screen_recording_start INTEGER,task_name TEXT,FOREIGN KEY (participant_id) REFERENCES participant(participant_id))");
    db.exec("CREATE TABLE IF NOT EXISTS fixation(fixation_id TEXT PRIMARY KEY,fixation_run_id INTEGER,fixation_start_event_time INTEGER,fixation_order_number INTEGER,x INTEGER,y INTEGER,fixation_target TEXT,source_file_line INTEGER, source_file_col INTEGER,token TEXT,syntactic_category TEXT,xpath TEXT,left_pupil_diameter REAL,right_pupil_diameter REAL,duration INTEGER)");
    db.exec("CREATE TABLE IF NOT EXISTS calibration(calibration_id INTEGER PRIMARY KEY)");
    db.exec("CREATE TABLE IF NOT EXISTS calibration_point(calibration_point_id TEXT,calibration_id INTEGER,calibration_x REAL,calibration_y REAL,FOREIGN KEY (calibration_id) REFERENCES calibration(calibration_id))");
    db.exec("CREATE TABLE IF NOT EXISTS calibration_sample(calibration_point_id TEXT,left_x REAL, left_y REAL,left_validation REAL,right_x REAL, right_y REAL,right_validation REAL,FOREIGN KEY (calibration_point_id) REFERENCES calibration_point(calibration_point_id))");
    db.exec("CREATE TABLE IF NOT EXISTS gaze(event_time INTEGER PRIMARY KEY,session_id INTEGER,calibration_id INTEGER,participant_id TEXT, tracker_time INTEGER, system_time INTEGER, x REAL, y REAL,left_x REAL, left_y REAL, left_pupil_diameter REAL, left_validation INTEGER,right_x REAL, right_y REAL, right_pupil_diameter REAL, right_validation INTEGER,user_left_x REAL,user_left_y REAL,user_left_z REAL,user_right_x REAL,user_right_y REAL,user_right_z REAL,FOREIGN KEY (session_id) REFERENCES session(session_id),FOREIGN KEY (calibration_id) REFERENCES calibration(calibration_id),FOREIGN KEY (participant_id) REFERENCES participant(participant_id))");
    db.exec("CREATE TABLE IF NOT EXISTS ide_context(event_time INTEGER,time_stamp TEXT,ide_type TEXT,gaze_target TEXT,gaze_target_type TEXT,source_file_path TEXT, source_file_line INTEGER, source_file_col INTEGER,editor_line_height REAL,editor_font_height REAL, editor_line_base_x REAL, editor_line_base_y REAL,source_token TEXT,source_token_type TEXT, source_token_xpath TEXT, source_token_syntactic_context TEXT, x REAL, y REAL,FOREIGN KEY (event_time) REFERENCES gaze(event_time))");
    db.exec("CREATE TABLE IF NOT EXISTS web_context(event_time INTEGER,browser_type TEXT,site_name TEXT,url TEXT,tag TEXT,FOREIGN KEY (event_time) REFERENCES gaze(event_time))");
    db.exec("CREATE TABLE IF NOT EXISTS fixation_gaze(fixation_id INTEGER,event_time INTEGER,FOREIGN KEY (fixation_id) REFERENCES fixation(fixation_id),FOREIGN KEY (event_time) REFERENCES gazes(event_time))");
    db.exec("CREATE TABLE IF NOT EXISTS files(file_hash TEXT PRIMARY KEY,session_id INTEGER,file_full_path TEXT,file_type TEXT,FOREIGN KEY (session_id) REFERENCES session(session_id))");
    db.commit();
}

void Database::close() {
    db.close();
}

QString Database::checkAndReturnError() {
    return db.lastError().text();
}

bool Database::isDatabaseOpen() {
    return db.isOpen();
}

bool Database::fileExists(const QString& file_hash) {
    QSqlQuery qry = db.exec(QString("SELECT file_hash FROM files WHERE file_hash = \"%1\"").arg(file_hash));
    qry.last();
    return qry.at() + 1 > 0;
}

bool Database::participantExists(const QString& participant_id) {
    QSqlQuery qry = db.exec(QString("SELECT participant_id FROM participant WHERE participant_id = \"%1\"").arg(participant_id));
    qry.last();
    return qry.at() + 1 > 0;
}

bool Database::calibrationExists(const QString& calibration_id) {
    QSqlQuery qry = db.exec(QString("SELECT calibration_id FROM calibration WHERE calibration_id = %1").arg(calibration_id));
    qry.last();
    return qry.at() + 1 > 0;
}

bool Database::pluginResponseExists(const QString& response_id) {
    QSqlQuery qry = db.exec(QString("SELECT event_time FROM ide_context WHERE event_time = %1").arg(response_id));
    qry.last();
    return qry.at() + 1 > 0;
}

void Database::startTransaction() { db.exec("BEGIN"); }

void Database::commit() { db.exec("COMMIT"); }

void Database::insertCalibration(QString calibration_id) {
    db.exec(QString("INSERT INTO calibration(calibration_id) VALUES(%1)").arg(calibration_id));
}

void Database::insertCalibrationPoint(QString calibration_point_id, QString calibration_id, QString calibration_x, QString calibration_y) {
    db.exec(QString("INSERT INTO calibration_point(calibration_point_id,calibration_id,calibration_x,calibration_y) VALUES(\"%1\",%2,%3,%4)").arg(calibration_point_id).arg(calibration_id).arg(calibration_x).arg(calibration_y));
}

void Database::insertCalibrationSample(QString calibration_point_id, QString left_x, QString left_y, QString left_validation, QString right_x, QString right_y, QString right_validation) {
    db.exec(QString("INSERT INTO calibration_sample(calibration_point_id,left_x,left_y,left_validation,right_x,right_y,right_validation) VALUES(\"%1\",%2,%3,%4,%5,%6,%7)").arg(calibration_point_id).arg(left_x).arg(left_y).arg(left_validation).arg(right_x).arg(right_y).arg(right_validation));
}

void Database::insertFile(QString file_hash, QString session_id, QString file_full_path, QString file_type) {
    db.exec(QString("INSERT INTO files(file_hash,session_id,file_full_path,file_type) VALUES(\"%1\",%2,\"%3\",\"%4\")").arg(file_hash).arg(session_id).arg(file_full_path).arg(file_type));
}

void Database::insertFixation(QString fixation_id, QString fixation_run_id, QString fixation_start_event_time, QString fixation_order_number, QString x, QString y, QString fixation_target, QString source_file_line, QString source_file_col, QString token, QString syntactic_category, QString xpath, QString left_pupil_diameter, QString right_pupil_diameter, QString duration) {
    //QString s = QString("INSERT INTO fixation(fixation_id,fixation_run_id,fixation_start_event_time,fixation_order_number,x,y,fixation_target,source_file_line,source_file_col,token,syntactic_category,xpath,left_pupil_diameter,right_pupil_diameter,duration) VALUES(\"%1\",%2,%3,%4,%5,%6,\"%7\",%8,%9,%10,%11,%12,%13,%14,%15)").arg(fixation_id).arg(fixation_run_id).arg(fixation_start_event_time).arg(fixation_order_number).arg(x).arg(y).arg(fixation_target).arg(source_file_line).arg(source_file_col).arg(token == "null" ? "null" : "\""+token+"\"").arg(syntactic_category == "null" ? "null" : "\""+syntactic_category+"\"").arg(xpath == "null" ? "null" : "\""+xpath+"\"").arg(left_pupil_diameter).arg(right_pupil_diameter).arg(duration);
    //std::cout << "Testing: " << s.toUtf8().constData() << std::endl;
    db.exec(QString("INSERT INTO fixation(fixation_id,fixation_run_id,fixation_start_event_time,fixation_order_number,x,y,fixation_target,source_file_line,source_file_col,token,syntactic_category,xpath,left_pupil_diameter,right_pupil_diameter,duration) VALUES(\"%1\",%2,%3,%4,%5,%6,\"%7\",%8,%9,%10,%11,%12,%13,%14,%15)").arg(fixation_id).arg(fixation_run_id).arg(fixation_start_event_time).arg(fixation_order_number).arg(x).arg(y).arg(fixation_target).arg(source_file_line).arg(source_file_col).arg(token == "null" ? "null" : "\""+token+"\"").arg(syntactic_category == "null" ? "null" : "\""+syntactic_category+"\"").arg(xpath == "null" ? "null" : "\""+xpath+"\"").arg(left_pupil_diameter).arg(right_pupil_diameter).arg(duration).replace("\"\"","\""));
}

void Database::insertFixationGaze(QString fixation_id, QString event_time) {
    db.exec(QString("INSERT INTO fixation_gaze(fixation_id,event_time) VALUES(\"%1\",%2)").arg(fixation_id).arg(event_time));
}

void Database::insertFixationRun(QString fixation_run_id, QString session_id, QString date_time, QString filter) {
    db.exec(QString("INSERT INTO fixation_run(fixation_run_id,session_id,date_time,filter) VALUES(%1,%2,%3,\"%4\")").arg(fixation_run_id).arg(session_id).arg(date_time).arg(filter));
}

void Database::insertGaze(QString event_time, QString session_id, QString calibration_id, QString participant_id, QString tracker_time, QString system_time, QString x, QString y, QString left_x, QString left_y, QString left_pupil_diameter, QString left_validation, QString right_x, QString right_y, QString right_pupil_diameter, QString right_validation, QString user_left_x, QString user_left_y, QString user_left_z, QString user_right_x, QString user_right_y, QString user_right_z) {
    db.exec(QString("INSERT INTO gaze(event_time,session_id,calibration_id,participant_id,tracker_time,system_time,x,y,left_x,left_y,left_pupil_diameter,left_validation,right_x,right_y,right_pupil_diameter,right_validation,user_left_x,user_left_y,user_left_z,user_right_x,user_right_y,user_right_z) VALUES(%1,%2,%3,\"%4\",\"%5\",\"%6\",\"%7\",\"%8\",\"%9\",\"%10\",\"%11\",\"%12\",\"%13\",\"%14\",\"%15\",\"%16\",\"%17\",\"%18\",\"%19\",\"%20\",\"%21\",\"%22\")").arg(event_time).arg(session_id).arg(calibration_id).arg(participant_id).arg(tracker_time).arg(system_time).arg(x).arg(y).arg(left_x).arg(left_y).arg(left_pupil_diameter).arg(left_validation).arg(right_x).arg(right_y).arg(right_pupil_diameter).arg(right_validation).arg(user_left_x).arg(user_left_y).arg(user_left_z).arg(user_right_x).arg(user_right_y).arg(user_right_z));
}

// The following parameters are unused and should always be inserted as "" - for now
// source_token, source_token_type, source_token_xpath, source_token_sytnactic_context
void Database::insertIDEContext(QString event_time, QString time_stamp, QString ide_type, QString gaze_target, QString gaze_target_type, QString source_file_path, QString source_file_line, QString source_file_col, QString editor_line_height, QString editor_font_height, QString editor_line_base_x, QString editor_line_base_y, QString source_token, QString source_token_type, QString source_token_xpath, QString source_token_syntactic_context, QString x, QString y) {
    db.exec(QString("INSERT INTO ide_context(event_time,time_stamp,ide_type,gaze_target,gaze_target_type,source_file_path,source_file_line,source_file_col,editor_line_height,editor_font_height,editor_line_base_x,editor_line_base_y,source_token,source_token_type,source_token_xpath,source_token_syntactic_context,x,y) VALUES(%1,\"%2\",\"%3\",\"%4\",\"%5\",\"%6\",%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18)").arg(event_time).arg(time_stamp).arg(ide_type).arg(gaze_target).arg(gaze_target_type).arg(source_file_path).arg(source_file_line).arg(source_file_col).arg(editor_line_height == "" ? "null" : editor_line_height).arg(editor_font_height == "" ? "null" : editor_font_height).arg(editor_line_base_x == "" ? "null" : editor_line_base_x).arg(editor_line_base_y == "" ? "null" : editor_line_base_y).arg(source_token == "" ? "null" : "\""+source_token+"\"").arg(source_token_type == "" ? "null" : "\""+source_token_type+"\"").arg(source_token_xpath == "" ? "null" : "\""+source_token_xpath+"\"").arg(source_token_syntactic_context == "" ? "null" : "\""+source_token_syntactic_context+"\"").arg(x).arg(y));
}



void Database::insertParticipant(QString participant_id, QString session_length) {
    db.exec(QString("INSERT INTO participant(participant_id,session_length) VALUES(\"%1\",%2)").arg(participant_id).arg(session_length));
}

void Database::insertSession(QString session_id, QString participant_id, QString screen_width, QString screen_height, QString tracker_type, QString tracker_serial_number, QString session_date, QString session_time, QString screen_recording_start, QString task_name) {
    db.exec(QString("INSERT INTO session(session_id,participant_id,screen_width,screen_height,tracker_type,tracker_serial_number,session_date,session_time,screen_recording_start,task_name) VALUES(%1,\"%2\",%3,%4,\"%5\",\"%6\",%7,%8,%9,\"%10\")").arg(session_id).arg(participant_id).arg(screen_width).arg(screen_height).arg(tracker_type).arg(tracker_serial_number).arg(session_date).arg(session_time).arg(screen_recording_start).arg(task_name));
}

// Currently unused
void Database::insertWebContext(QString event_time, QString browser_type, QString site_name, QString url, QString tag) {
    db.exec(QString("INSERT INTO web_context(event_time,browser_type,site_name,url,tag) VALUES(%1,\"%2\",\"%3\",\"%4\",\"%5\")").arg(event_time).arg(browser_type).arg(site_name).arg(url).arg(tag));
}

QString Database::getSessionFromParticipantAndTask(QString participant_id, QString task_name) {
    QSqlQuery session_id = db.exec(QString("SELECT session_id FROM session WHERE participant_id = \"%1\" AND task_name = \"%2\"").arg(participant_id).arg(task_name));
    session_id.first();
    return session_id.value(0).toString();
}

QVector<QString> Database::getSessions() {
    QVector<QString> data;
    QSqlQuery sessions = db.exec("SELECT participant_id, task_name FROM session");
    while(sessions.next()) {
        data.push_back(sessions.value(0).toString() + " - " + sessions.value(1).toString());
    }
    return data;
}

QVector<QString> Database::getGazeTargetsFromSession(QString session_id) {
/*
    QSqlQuery targets = db.exec(QString("SELECT DISTINCT ide_context.gaze_target FROM ide_context JOIN gaze ON gaze.event_time=ide_context.event_time WHERE ide_context.gaze_target != \"\" AND gaze.session_id = %1").arg(session_id));
    QVector<QString> gaze_targets;
    while(targets.next()) { gaze_targets.push_back(targets.value(0).toString()); }
    return gaze_targets;
    //*/


    QSqlQuery targets = db.exec(QString("SELECT DISTINCT ide_context.gaze_target FROM ide_context WHERE ide_context.gaze_target != \"\""));
    QVector<QString> gaze_targets;
    while(targets.next()) { gaze_targets.push_back(targets.value(0).toString()); }
    return gaze_targets;
    //*/
}

QVector<Gaze> Database::getGazesFromSessionAndTarget(QString session_id, QString gaze_target) {

    QVector<Gaze> gazes;
    QSqlQuery session_gazes = db.exec(QString("SELECT gaze.event_time, gaze.x, gaze.y, gaze.system_time, gaze.left_pupil_diameter, gaze.right_pupil_diameter, gaze.left_validation, gaze.right_validation, ide_context.gaze_target, ide_context.gaze_target_type, ide_context.source_file_line, ide_context.source_file_col, ide_context.source_token, ide_context.source_token_xpath, ide_context.source_token_syntactic_context FROM gaze JOIN ide_context ON gaze.event_time=ide_context.event_time WHERE gaze.session_id = %1 AND ide_context.gaze_target = \"%2\" ORDER BY gaze.event_time ASC").arg(session_id).arg(gaze_target));
    Gaze last_valid = Gaze();
    while(session_gazes.next()) {
        Gaze data(session_gazes);
        if(data.isValid()) {
            gazes.push_back(data);
            last_valid = data;
        }
        else { if(last_valid.isValid()) { gazes.push_back(last_valid); } }
    }

    return gazes;

}

QVector<std::pair<QString, QString> > Database::getFilesViewed() {
    QSqlQuery data = db.exec("SELECT DISTINCT gaze_target, source_file_path FROM ide_context WHERE gaze_target is not \"\"");
    QVector<std::pair<QString,QString>> files_viewed;
    while(data.next()) {
        files_viewed.push_back(std::make_pair(data.value(0).toString(),data.value(1).toString()));
    }
    return files_viewed;
}

QVector<QVector<QString>> Database::getGazesForSyntacticMapping(QString file_path, bool overwrite) {
    QVector<QVector<QString>> gazes;
    QSqlQuery data = db.exec(QString("SELECT event_time,source_file_line,source_file_col,source_token_syntactic_context FROM ide_context WHERE source_file_path = \"%1\" AND source_file_line >= 0 AND source_file_line IS NOT NULL AND source_file_col >= 0 AND source_file_col IS NOT NULL").arg(file_path) + (overwrite ? " AND source_token_syntactic_context IS NULL " : " ") + QString("ORDER BY source_file_line ASC, source_file_col ASC"));
    while(data.next()) {
        QVector<QString> hold;
        for(int i = 0; i < 4; ++i) { hold.push_back(data.value(i).toString()); }
        gazes.push_back(hold);
    }
    return gazes;
}

QVector<QVector<QString> > Database::getGazesForSourceMapping(QString file_path, bool overwrite) {
    QVector<QVector<QString>> gazes;
    QSqlQuery data = db.exec(QString("SELECT event_time,source_file_line,source_file_col,source_token_syntactic_context FROM ide_context WHERE source_file_path = \"%1\" AND source_file_line >= 0 AND source_file_line IS NOT NULL AND source_file_col >= 0 AND source_file_col IS NOT NULL").arg(file_path) + (overwrite ? " AND source_token_syntactic_context IS NOT NULL AND source_token IS NULL " : " ") + QString("ORDER BY source_file_line ASC, source_file_col ASC"));
    while(data.next()) {
        QVector<QString> hold;
        for(int i = 0; i < 4; ++i) { hold.push_back(data.value(i).toString()); }
        gazes.push_back(hold);
    }
    return gazes;
}

QVector<QString> Database::getFixationRunIDs() {
    QVector<QString> ids;
    QSqlQuery data = db.exec("SELECT DISTINCT fixation_run_id FROM fixation");
    while(data.next()) {
        ids.push_back(data.value(0).toString());
    }
    return ids;
}

QVector<QVector<QString> > Database::getFixationsFromRunID(QString run_id) {
    QVector<QVector<QString>> fixations;
    QSqlQuery data = db.exec(QString("SELECT DISTINCT xpath, source_file_line, source_file_col, token FROM fixation WHERE fixation_run_id = %1 AND syntactic_category != '' AND token != 'WHITESPACE' ORDER BY source_file_line ASC, source_file_col ASC").arg(run_id));
    while(data.next()) {
        QVector<QString> hold;
        for(int i = 0; i < 4; ++i) { hold.push_back(data.value(i).toString()); }
        fixations.push_back(hold);
    }
    return fixations;
}

void Database::updateGazeWithSyntacticInfo(QString event_id, QString xpath, QString syntactic_context) {
    xpath.replace("\"","'");
    db.exec(QString("UPDATE ide_context SET source_token_xpath = \"%1\", source_token_syntactic_context = \"%2\" WHERE event_time = %3").arg(xpath).arg(syntactic_context).arg(event_id));
}

void Database::updateGazeWithTokenInfo(QString event_id, QString token, QString token_type) {
    db.exec(QString("UPDATE ide_context SET source_token = '%1',source_token_type = %2 WHERE event_time = %3").arg(token.replace("'","''")).arg(token_type == "" ? "null" : "\""+token_type+"\"").arg(event_id));
}

QVector<QVector<QString> > Database::runFilterQuery(QString query) {
    QVector<QVector<QString>> fixations;
    QSqlQuery data = db.exec(query);
    while(data.next()) {
        QVector<QString> hold;
        for(int i = 0; i < 15; ++i) { hold.push_back((data.value(i).toString())); }
        fixations.push_back(hold);
    }
    return fixations;
}



