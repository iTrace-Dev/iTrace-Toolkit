#include "database.h"

Database::Database() {
    db = QSqlDatabase::addDatabase("QSQLITE");
}

Database::Database(QString fileURL) : Database() {
    file_path = fileURL;
    file_path.remove("file:///");

    db.setDatabaseName(file_path.toUtf8().constData());
    db.open();
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
    db.exec(QString("INSERT INTO fixation(fixation_id,fixation_run_id,fixation_start_event_time,fixation_order_number,x,y,fixation_target,source_file_line,source_file_col,token,syntactic_category,xpath,left_pupil_diameter,right_pupil_diameter,duration) VALUES(\"%1\",%2,%3,%4,%5,%6,\"%7\",%8,%9,%10,%11,%12,%13,%14,%15)").arg(fixation_id).arg(fixation_run_id).arg(fixation_start_event_time).arg(fixation_order_number).arg(x).arg(y).arg(fixation_target).arg(source_file_line).arg(source_file_col).arg(token == "" ? "null" : "\""+token+"\"").arg(syntactic_category == "" ? "null" : "\""+syntactic_category+"\"").arg(xpath == "" ? "null" : "\""+xpath+"\"").arg(left_pupil_diameter).arg(right_pupil_diameter).arg(duration));
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
void Database::insertIDEContext(QString event_time, QString time_stamp, QString ide_type, QString gaze_target, QString gaze_target_type, QString source_file_path, QString source_file_line, QString source_file_col, QString editor_line_height, QString editor_font_height, QString editor_line_base_x, QString editor_line_base_y, QString source_token, QString source_token_type, QString source_token_xpath, QString source_token_syntactic_context) {
    db.exec(QString("INSERT INTO ide_context(event_time,time_stamp,ide_type,gaze_target,gaze_target_type,source_file_path,source_file_line,source_file_col,editor_line_height,editor_font_height,editor_line_base_x,editor_line_base_y) VALUES(%1,\"%2\",\"%3\",\"%4\",\"%5\",\"%6\",%7,%8,%9,%10,%11,%12,%13,%14,%15,%16)").arg(event_time).arg(time_stamp).arg(ide_type).arg(gaze_target).arg(gaze_target_type).arg(source_file_path).arg(source_file_line).arg(source_file_col).arg(editor_line_height == "" ? "null" : editor_line_height).arg(editor_font_height == "" ? "null" : editor_font_height).arg(editor_line_base_x == "" ? "null" : editor_line_base_x).arg(editor_line_base_y == "" ? "null" : editor_line_base_y).arg(source_token == "" ? "null" : "\""+source_token+"\"").arg(source_token_type == "" ? "null" : "\""+source_token_type+"\"").arg(source_token_xpath == "" ? "null" : "\""+source_token_xpath+"\"").arg(source_token_syntactic_context == "" ? "null" : "\""+source_token_syntactic_context+"\""));
}

void Database::insertParticipant(QString participant_id, QString session_length) {
    db.exec(QString("INSERT INTO participant(participant_id,session_length) VALUES(\"%1\",%2)").arg(participant_id).arg(session_length));
}

void Database::insertSession(QString session_id, QString participant_id, QString screen_width, QString screen_height, QString tracker_type, QString tracker_serial_number, QString session_date, QString session_time, QString screen_recording_start, QString task_name) {
    db.exec(QString("INSERT INTO session(session_id,participant_id,screen_width,screen_height,tracker_type,tracker_serial_number,session_date,session_time.screen_recording_start,task_name) VALUES(%1,\"%2\",%3,%4,\"%5\",\"%6\",%7,%8,\"%9\")").arg(session_id).arg(participant_id).arg(screen_width).arg(screen_height).arg(tracker_type).arg(tracker_serial_number).arg(session_date).arg(session_time).arg(screen_recording_start).arg(task_name));
}

// Currently unused
void Database::insertWebContext(QString event_time, QString browser_type, QString site_name, QString url, QString tag) {
    db.exec(QString("INSERT INTO web_context(event_time,browser_type,site_name,url,tag) VALUES(%1,\"%2\",\"%3\",\"%4\",\"%5\")").arg(event_time).arg(browser_type).arg(site_name).arg(url).arg(tag));
}

