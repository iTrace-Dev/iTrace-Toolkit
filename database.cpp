/********************************************************************************************************************************************************
* @file database.cpp
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#include "database.h"

Database::Database() { }

Database::Database(QString file_path) : Database() {
    int error = sqlite3_open(file_path.toStdString().c_str(), &db);
    open = true;

    std::cout << error << std::endl;

    QString pragma_query =
            "pragma journal_mode = WAL;"
            "pragma synchronous = off;"
            "pragma temp_store = memory;"
            "pragma mmap_size = 30000000000;"
            ;

    QString table_query =
            "CREATE TABLE IF NOT EXISTS participant(participant_id TEXT PRIMARY KEY,session_length INTEGER);"
            "CREATE TABLE IF NOT EXISTS fixation_run(fixation_run_id INTEGER PRIMARY KEY,session_id INTEGER,date_time INTEGER,filter TEXT,FOREIGN KEY (session_id) REFERENCES session(session_id));"
            "CREATE TABLE IF NOT EXISTS session(session_id INTEGER PRIMARY KEY,participant_id TEXT,screen_width INTEGER, screen_height INTEGER,tracker_type TEXT, tracker_serial_number TEXT,session_date INTEGER, session_time INTEGER,screen_recording_start INTEGER,task_name TEXT,FOREIGN KEY (participant_id) REFERENCES participant(participant_id));"
            "CREATE TABLE IF NOT EXISTS fixation(fixation_id TEXT PRIMARY KEY,fixation_run_id INTEGER,fixation_start_event_time INTEGER,fixation_order_number INTEGER,x INTEGER,y INTEGER,fixation_target TEXT,source_file_line INTEGER, source_file_col INTEGER,token TEXT,syntactic_category TEXT,xpath TEXT,left_pupil_diameter REAL,right_pupil_diameter REAL,duration INTEGER);"
            "CREATE TABLE IF NOT EXISTS calibration(calibration_id INTEGER PRIMARY KEY);"
            "CREATE TABLE IF NOT EXISTS calibration_point(calibration_point_id TEXT,calibration_id INTEGER,calibration_x REAL,calibration_y REAL,FOREIGN KEY (calibration_id) REFERENCES calibration(calibration_id));"
            "CREATE TABLE IF NOT EXISTS calibration_sample(calibration_point_id TEXT,left_x REAL, left_y REAL,left_validation REAL,right_x REAL, right_y REAL,right_validation REAL,FOREIGN KEY (calibration_point_id) REFERENCES calibration_point(calibration_point_id));"
            "CREATE TABLE IF NOT EXISTS gaze(event_time INTEGER PRIMARY KEY,session_id INTEGER,calibration_id INTEGER,participant_id TEXT, tracker_time INTEGER, system_time INTEGER, x REAL, y REAL,left_x REAL, left_y REAL, left_pupil_diameter REAL, left_validation INTEGER,right_x REAL, right_y REAL, right_pupil_diameter REAL, right_validation INTEGER,user_left_x REAL,user_left_y REAL,user_left_z REAL,user_right_x REAL,user_right_y REAL,user_right_z REAL,FOREIGN KEY (session_id) REFERENCES session(session_id),FOREIGN KEY (calibration_id) REFERENCES calibration(calibration_id),FOREIGN KEY (participant_id) REFERENCES participant(participant_id));"
            "CREATE TABLE IF NOT EXISTS ide_context(event_time INTEGER,time_stamp TEXT,ide_type TEXT,gaze_target TEXT,gaze_target_type TEXT,source_file_path TEXT, source_file_line INTEGER, source_file_col INTEGER,editor_line_height REAL,editor_font_height REAL, editor_line_base_x REAL, editor_line_base_y REAL,source_token TEXT,source_token_type TEXT, source_token_xpath TEXT, source_token_syntactic_context TEXT, x REAL, y REAL,FOREIGN KEY (event_time) REFERENCES gaze(event_time));"
            "CREATE TABLE IF NOT EXISTS web_context(event_time INTEGER,browser_type TEXT,site_name TEXT,url TEXT,tag TEXT,FOREIGN KEY (event_time) REFERENCES gaze(event_time));"
            "CREATE TABLE IF NOT EXISTS fixation_gaze(fixation_id INTEGER,event_time INTEGER,FOREIGN KEY (fixation_id) REFERENCES fixation(fixation_id),FOREIGN KEY (event_time) REFERENCES gazes(event_time));"
            "CREATE TABLE IF NOT EXISTS files(file_hash TEXT PRIMARY KEY,session_id INTEGER,file_full_path TEXT,file_type TEXT,FOREIGN KEY (session_id) REFERENCES session(session_id));"

            "CREATE INDEX idx_event_time ON ide_context(event_time);"
            ;


    //sqlite3_exec(db, pragma_query.toStdString().c_str(), NULL, 0, NULL);

    sqlite3_exec(db, table_query.toStdString().c_str(), NULL, 0, NULL);

    commit();
}

void Database::close() {
    if(open) {
        sqlite3_close(db);
        open = false;
    }
}

bool Database::isDatabaseOpen() {
    return open;
}

int exists_callback(void *count, int argc, char** argv, char** azColName) {
    int* x = (int*)count;
    *x = argc;
    return 0;
}

bool Database::fileExists(const QString& file_hash) {
    QString query = QString("SELECT file_hash FROM files WHERE file_hash = \"%1\";").arg(file_hash);
    int count = 0;
    sqlite3_exec(db, query.toStdString().c_str(), exists_callback, (void*)&count, NULL);
    return count;
}

bool Database::participantExists(const QString& participant_id) {
    QString query = QString("SELECT participant_id FROM participant WHERE participant_id = \"%1\";").arg(participant_id);
    int count = 0;
    sqlite3_exec(db, query.toStdString().c_str(), exists_callback, (void*)&count, NULL);
    return count;
}

bool Database::calibrationExists(const QString& calibration_id) {
    QString query = QString("SELECT calibration_id FROM calibration WHERE calibration_id = %1;").arg(calibration_id);
    int count = 0;
    sqlite3_exec(db, query.toStdString().c_str(), exists_callback, (void*)&count, NULL);
    return count;
}

bool Database::pluginResponseExists(const QString& response_id) {
    QString query = QString("SELECT COUNT(*) FROM ide_context WHERE event_time = %1;").arg(response_id);
    int count = 0;
    sqlite3_exec(db, query.toStdString().c_str(), exists_callback, (void*)&count, NULL);
    return count;
}

void Database::startTransaction() {
    sqlite3_exec(db,"BEGIN;",NULL,0,NULL);
    //db.exec("BEGIN");
}

void Database::commit() {
    sqlite3_exec(db,"COMMIT;",NULL,0,NULL);
    //db.exec("COMMIT");
}

void Database::insertCalibration(QString calibration_id) {
    QString query = QString("INSERT INTO calibration(calibration_id) VALUES(%1);").arg(calibration_id);
    sqlite3_exec(db,query.toStdString().c_str(),NULL,0,NULL);
}

void Database::insertCalibrationPoint(QString calibration_point_id, QString calibration_id, QString calibration_x, QString calibration_y) {
    QString query = QString("INSERT INTO calibration_point(calibration_point_id,calibration_id,calibration_x,calibration_y) VALUES(\"%1\",%2,%3,%4);").arg(calibration_point_id,calibration_id,calibration_x,calibration_y);
    sqlite3_exec(db,query.toStdString().c_str(),NULL,0,NULL);
}

void Database::insertCalibrationSample(QString calibration_point_id, QString left_x, QString left_y, QString left_validation, QString right_x, QString right_y, QString right_validation) {
    QString query = QString("INSERT INTO calibration_sample(calibration_point_id,left_x,left_y,left_validation,right_x,right_y,right_validation) VALUES(\"%1\",%2,%3,%4,%5,%6,%7)").arg(calibration_point_id,left_x,left_y,left_validation,right_x,right_y,right_validation);
    sqlite3_exec(db,query.toStdString().c_str(),NULL,0,NULL);
}

void Database::insertFile(QString file_hash, QString session_id, QString file_full_path, QString file_type) {
    QString query = QString("INSERT INTO files(file_hash,session_id,file_full_path,file_type) VALUES(\"%1\",%2,\"%3\",\"%4\");").arg(file_hash,session_id,file_full_path,file_type);
    sqlite3_exec(db,query.toStdString().c_str(),NULL,0,NULL);
}

void Database::insertFixation(QString fixation_id, QString fixation_run_id, QString fixation_start_event_time, QString fixation_order_number, QString x, QString y, QString fixation_target, QString source_file_line, QString source_file_col, QString token, QString syntactic_category, QString xpath, QString left_pupil_diameter, QString right_pupil_diameter, QString duration) {
    QString query = QString("INSERT INTO fixation(fixation_id,fixation_run_id,fixation_start_event_time,fixation_order_number,x,y,fixation_target,source_file_line,source_file_col,token,syntactic_category,xpath,left_pupil_diameter,right_pupil_diameter,duration) VALUES(\"%1\",%2,%3,%4,%5,%6,\"%7\",%8,%9,%10,%11,%12,%13,%14,%15);").arg(fixation_id,fixation_run_id,fixation_start_event_time,fixation_order_number,x,y,fixation_target,source_file_line,source_file_col,token == "null" ? "null" : "\""+token+"\"",syntactic_category == "null" ? "null" : "\""+syntactic_category+"\"",xpath == "null" ? "null" : "\""+xpath+"\"",left_pupil_diameter,right_pupil_diameter,duration).replace("\"\"","\"");
    sqlite3_exec(db,query.toStdString().c_str(),NULL,0,NULL);
}

void Database::insertFixationGaze(QString fixation_id, QString event_time) {
    QString query = QString("INSERT INTO fixation_gaze(fixation_id,event_time) VALUES(\"%1\",%2);").arg(fixation_id,event_time);
    sqlite3_exec(db,query.toStdString().c_str(),NULL,0,NULL);
}

void Database::insertFixationRun(QString fixation_run_id, QString session_id, QString date_time, QString filter) {
    QString query = QString("INSERT INTO fixation_run(fixation_run_id,session_id,date_time,filter) VALUES(%1,%2,%3,\"%4\");").arg(fixation_run_id,session_id,date_time,filter);
    sqlite3_exec(db,query.toStdString().c_str(),NULL,0,NULL);
}

void Database::insertGaze(QString event_time, QString session_id, QString calibration_id, QString participant_id, QString tracker_time, QString system_time, QString x, QString y, QString left_x, QString left_y, QString left_pupil_diameter, QString left_validation, QString right_x, QString right_y, QString right_pupil_diameter, QString right_validation, QString user_left_x, QString user_left_y, QString user_left_z, QString user_right_x, QString user_right_y, QString user_right_z) {
    QString query = QString("INSERT INTO gaze(event_time,session_id,calibration_id,participant_id,tracker_time,system_time,x,y,left_x,left_y,left_pupil_diameter,left_validation,right_x,right_y,right_pupil_diameter,right_validation,user_left_x,user_left_y,user_left_z,user_right_x,user_right_y,user_right_z) VALUES(%1,%2,%3,\"%4\",\"%5\",\"%6\",\"%7\",\"%8\",\"%9\",\"%10\",\"%11\",\"%12\",\"%13\",\"%14\",\"%15\",\"%16\",\"%17\",\"%18\",\"%19\",\"%20\",\"%21\",\"%22\");").arg(event_time,session_id,calibration_id,participant_id,tracker_time,system_time,x,y,left_x,left_y,left_pupil_diameter,left_validation,right_x,right_y,right_pupil_diameter,right_validation,user_left_x,user_left_y,user_left_z,user_right_x,user_right_y,user_right_z);
    sqlite3_exec(db,query.toStdString().c_str(),NULL,0,NULL);
}

// The following parameters are unused here and should always be inserted as ""
// source_token, source_token_type, source_token_xpath, source_token_sytnactic_context
void Database::insertIDEContext(QString event_time, QString time_stamp, QString ide_type, QString gaze_target, QString gaze_target_type, QString source_file_path, QString source_file_line, QString source_file_col, QString editor_line_height, QString editor_font_height, QString editor_line_base_x, QString editor_line_base_y, QString source_token, QString source_token_type, QString source_token_xpath, QString source_token_syntactic_context, QString x, QString y) {
    QString query = QString("INSERT INTO ide_context(event_time,time_stamp,ide_type,gaze_target,gaze_target_type,source_file_path,source_file_line,source_file_col,editor_line_height,editor_font_height,editor_line_base_x,editor_line_base_y,source_token,source_token_type,source_token_xpath,source_token_syntactic_context,x,y) VALUES(%1,\"%2\",\"%3\",\"%4\",\"%5\",\"%6\",%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18);").arg(event_time,time_stamp,ide_type,gaze_target,gaze_target_type,source_file_path,source_file_line,source_file_col,editor_line_height == "" ? "null" : editor_line_height,editor_font_height == "" ? "null" : editor_font_height,editor_line_base_x == "" ? "null" : editor_line_base_x,editor_line_base_y == "" ? "null" : editor_line_base_y,source_token == "" ? "null" : "\""+source_token+"\"",source_token_type == "" ? "null" : "\""+source_token_type+"\"",source_token_xpath == "" ? "null" : "\""+source_token_xpath+"\"",source_token_syntactic_context == "" ? "null" : "\""+source_token_syntactic_context+"\"",x,y);
    sqlite3_exec(db,query.toStdString().c_str(),NULL,0,NULL);
}

void Database::insertParticipant(QString participant_id, QString session_length) {
    QString query = QString("INSERT INTO participant(participant_id,session_length) VALUES(\"%1\",%2);").arg(participant_id,session_length);
    sqlite3_exec(db,query.toStdString().c_str(),NULL,0,NULL);
}

void Database::insertSession(QString session_id, QString participant_id, QString screen_width, QString screen_height, QString tracker_type, QString tracker_serial_number, QString session_date, QString session_time, QString screen_recording_start, QString task_name) {
    QString query = QString("INSERT INTO session(session_id,participant_id,screen_width,screen_height,tracker_type,tracker_serial_number,session_date,session_time,screen_recording_start,task_name) VALUES(%1,\"%2\",%3,%4,\"%5\",\"%6\",%7,%8,%9,\"%10\");").arg(session_id,participant_id,screen_width,screen_height,tracker_type,tracker_serial_number,session_date,session_time,screen_recording_start,task_name);
    sqlite3_exec(db,query.toStdString().c_str(),NULL,0,NULL);
}

// Currently unused
void Database::insertWebContext(QString event_time, QString browser_type, QString site_name, QString url, QString tag) {
    QString query = QString("INSERT INTO web_context(event_time,browser_type,site_name,url,tag) VALUES(%1,\"%2\",\"%3\",\"%4\",\"%5\");").arg(event_time,browser_type,site_name,url,tag);
    sqlite3_exec(db,query.toStdString().c_str(),NULL,0,NULL);
}

int getSessionsCALLBACK(void *sessions, int argc, char** argv, char** azColName) {
    QVector<QString>* x = (QVector<QString>*)sessions;
    x->push_back(argv[0] + QString(" - ") + argv[1]);
    return 0;
}
QVector<QString> Database::getSessions() {
    QString query = QString("SELECT participant_id, task_name FROM session;");
    QVector<QString> data;
    sqlite3_exec(db, query.toStdString().c_str(), getSessionsCALLBACK, (void*)&data, NULL);
    return data;
}

int getAllIDEContextIDsCALLBACK(void *ids, int argc, char** argv, char** azColName) {
    QVector<QString>* x = (QVector<QString>*)ids;
    x->push_back(argv[0]);
    return 0;
}
QVector<QString> Database::getAllIDEContextIDs() {
    QString query = QString("SELECT event_time FROM ide_context;");
    QVector<QString> data;
    sqlite3_exec(db, query.toStdString().c_str(), getAllIDEContextIDsCALLBACK, (void*)&data, NULL);
    return data;
}

int getGazesForSyntacticMappingCALLBACK(void *gazes, int argc, char** argv, char** azColName) {
    QVector<QVector<QString>>* x = (QVector<QVector<QString>>*)gazes;
    QVector<QString> hold;
    for(int i = 0; i < argc; ++i) { hold.push_back(argv[i]); }
    x->push_back(hold);
    return 0;
}
QVector<QVector<QString>> Database::getGazesForSyntacticMapping(QString file_path, bool overwrite) {
    QString query = QString("SELECT event_time,source_file_line,source_file_col,source_token_syntactic_context FROM ide_context WHERE source_file_path = \"%1\" AND source_file_line >= 0 AND source_file_line IS NOT NULL AND source_file_col >= 0 AND source_file_col IS NOT NULL").arg(file_path) + (overwrite ? " AND source_token_syntactic_context IS NULL " : " ") + QString("ORDER BY source_file_line ASC, source_file_col ASC;");
    QVector<QVector<QString>> data;
    sqlite3_exec(db, query.toStdString().c_str(), getGazesForSyntacticMappingCALLBACK, (void*)&data, NULL);
    return data;
}

int getGazesForSourceMappingCALLBACK(void *gazes, int argc, char** argv, char** azColName) {
    QVector<QVector<QString>>* x = (QVector<QVector<QString>>*)gazes;
    QVector<QString> hold;
    for(int i = 0; i < argc; ++i) { hold.push_back(argv[i]); }
    x->push_back(hold);
    return 0;
}
QVector<QVector<QString>> Database::getGazesForSourceMapping(QString file_path, bool overwrite) {
    QString query = QString("SELECT event_time,source_file_line,source_file_col,source_token_syntactic_context FROM ide_context WHERE source_file_path = \"%1\" AND source_file_line >= 0 AND source_file_line IS NOT NULL AND source_file_col >= 0 AND source_file_col IS NOT NULL").arg(file_path) + (overwrite ? " AND source_token_syntactic_context IS NOT NULL AND source_token IS NULL " : " ") + QString("ORDER BY source_file_line ASC, source_file_col ASC;");
    QVector<QVector<QString>> data;
    sqlite3_exec(db, query.toStdString().c_str(), getGazesForSourceMappingCALLBACK, (void*)&data, NULL);
    return data;
}

int getGazeTargetsFromSessionCALLBACK(void *gaze_targets, int argc, char** argv, char** azColName) {
    QVector<QString>* x = (QVector<QString>*)gaze_targets;
    x->push_back(argv[0]);
    return 0;
}
QVector<QString> Database::getGazeTargetsFromSession(QString session_id) {
    QString query = QString("SELECT DISTINCT ide_context.gaze_target FROM ide_context WHERE ide_context.gaze_target != \"\";");
    QVector<QString> gaze_targets;
    sqlite3_exec(db, query.toStdString().c_str(), getGazeTargetsFromSessionCALLBACK, (void*)&gaze_targets, NULL);
    return gaze_targets;
}

int getGazesFromSessionAndTargetCALLBACK(void *gazes, int argc, char** argv, char** azColName) {
    QVector<Gaze>* x = (QVector<Gaze>*)gazes;
    Gaze data(argv);
    if(data.isValid()) {
        x->push_back(data);
    }
    else {
        if(x->size() != 0) {
            x->push_back(Gaze(x->last()));
        }
        else {
            x->push_back(Gaze());
        }
    }
    return 0;
}
QVector<Gaze> Database::getGazesFromSessionAndTarget(QString session_id, QString gaze_target) {
    QVector<Gaze> gazes;
    QString query = QString("SELECT gaze.event_time, gaze.x, gaze.y, gaze.system_time, gaze.left_pupil_diameter, gaze.right_pupil_diameter, gaze.left_validation, gaze.right_validation, ide_context.gaze_target, ide_context.gaze_target_type, ide_context.source_file_line, ide_context.source_file_col, ide_context.source_token, ide_context.source_token_xpath, ide_context.source_token_syntactic_context FROM gaze JOIN ide_context ON gaze.event_time=ide_context.event_time WHERE gaze.session_id = %1 AND ide_context.gaze_target = \"%2\" ORDER BY gaze.event_time ASC;").arg(session_id,gaze_target);
    sqlite3_exec(db, query.toStdString().c_str(), getGazesFromSessionAndTargetCALLBACK, (void*)&gazes, NULL);
    return gazes;
}

int getFilesViewedCALLBACK(void *files_viewed, int argc, char** argv, char** azColName) {
    QVector<std::pair<QString,QString>>* x = (QVector<std::pair<QString,QString>>*)files_viewed;
    x->push_back(std::make_pair(QString(argv[0]),QString(argv[1])));
    return 0;
}
QVector<std::pair<QString, QString> > Database::getFilesViewed() {
    QString query = QString("SELECT DISTINCT gaze_target, source_file_path FROM ide_context WHERE gaze_target is not \"\";");
    QVector<std::pair<QString,QString>> files_viewed;
    sqlite3_exec(db, query.toStdString().c_str(), getFilesViewedCALLBACK, (void*)&files_viewed, NULL);
    return files_viewed;
}


int getSessionFromParticipantAndTaskCALLBACK(void *id, int argc, char** argv, char** azColName) {
    QString* x = (QString*)id;
    *x = argv[0];
    return 0;
}
QString Database::getSessionFromParticipantAndTask(QString participant_id, QString task_name) {
    QString query = QString("SELECT session_id FROM session WHERE participant_id = \"%1\" AND task_name = \"%2\";").arg(participant_id,task_name);
    QString id;
    sqlite3_exec(db, query.toStdString().c_str(), getSessionFromParticipantAndTaskCALLBACK, (void*)&id, NULL);
    return id;
}


void Database::updateGazeWithSyntacticInfo(QString event_id, QString xpath, QString syntactic_context) {
    xpath.replace("\"","'");
    QString query = QString("UPDATE ide_context SET source_token_xpath = \"%1\", source_token_syntactic_context = \"%2\" WHERE event_time = %3;").arg(xpath,syntactic_context,event_id);
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db, query.toStdString().c_str(),NULL,0,&zErrMsg);
    //std::cout << "rc: " << rc << std::endl;
    if(rc != SQLITE_OK) {
        std::cout << "updateGazeWithSyntacticInfo" << std::endl;
        std::cout << query.toStdString() << std::endl;
        std::cout << "Error: " << QString(zErrMsg).toStdString() << std::endl;
    }
}

void Database::updateGazeWithTokenInfo(QString event_id, QString token, QString token_type) {
    QString query = QString("UPDATE ide_context SET source_token = '%1',source_token_type = %2 WHERE event_time = %3;").arg(token.replace("'","''"),token_type == "" ? "null" : "\""+token_type+"\"",event_id);
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db, query.toStdString().c_str(),NULL,0,&zErrMsg);
    if(rc != SQLITE_OK) {
        std::cout << "updateGazeWithTokenInfo" << std::endl;
        std::cout << query.toStdString() << std::endl;
        std::cout << "Error: " << QString(zErrMsg).toStdString() << std::endl;
    }
}

QString Database::queryUpdateGazeWithSyntacticInfo(QString event_id, QString xpath, QString syntactic_context) {
    return QString("UPDATE ide_context SET source_token_xpath = \"%1\", source_token_syntactic_context = \"%2\" WHERE event_time = %3;").arg(xpath,syntactic_context,event_id);
}

QString Database::queryUpdateGazeWithTokenInfo(QString event_id, QString token, QString token_type) {
    return QString("UPDATE ide_context SET source_token = '%1',source_token_type = %2 WHERE event_time = %3;").arg(token.replace("'","''"),token_type == "" ? "null" : "\""+token_type+"\"",event_id);
}

int runFilterQueryCALLBACK(void *fixs, int argc, char** argv, char** azColName) {
    QVector<QVector<QString>>* x = (QVector<QVector<QString>>*)fixs;
    QVector<QString> hold;
    for(int i = 0; i < argc; ++i) { hold.push_back(argv[i]); }
    x->push_back(hold);
    return 0;
}
QVector<QVector<QString> > Database::runFilterQuery(QString query) {
    QVector<QVector<QString>> fixations;
    sqlite3_exec(db, query.toStdString().c_str(), runFilterQueryCALLBACK, (void*)&fixations, NULL);
    return fixations;
}

void Database::executeLongUpdateQuery(QString big_query) {
    sqlite3_exec(db,big_query.toStdString().c_str(),NULL,0,NULL);
}

// TODO - Need to port these from QSqlDatabase to sqlite3 C Library

/*

QString Database::checkAndReturnError() {
    return db.lastError().text();
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

*/



