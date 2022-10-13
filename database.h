/********************************************************************************************************************************************************
* @file database.h
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QVector>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "gaze.h"

#include <iostream>


class Database {
public:
    Database();
    Database(QString);

    void close();
    
    QString checkAndReturnError();
    
    bool isDatabaseOpen();
    bool fileExists(const QString&);
    bool participantExists(const QString&);
    bool calibrationExists(const QString&);
    bool pluginResponseExists(const QString&);
    
    void startTransaction();
    void commit();

    void executeLongQuery(QString);
    
    void insertCalibration(QString);
    void insertCalibrationPoint(QString,QString,QString,QString);
    void insertCalibrationSample(QString,QString,QString,QString,QString,QString,QString);
    void insertFile(QString,QString,QString,QString);
    void insertFixation(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString);
    void insertFixationGaze(QString,QString);
    void insertFixationRun(QString,QString,QString,QString);
    void insertGaze(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString);
    void insertIDEContext(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString);
    void insertParticipant(QString,QString);
    void insertSession(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString);
    void insertWebContext(QString,QString,QString,QString,QString);

    QString getSessionFromParticipantAndTask(QString,QString);
    QVector<QString> getSessions();
    QVector<QString> getAllIDEContextIDs();
    QVector<QString> getGazeTargetsFromSession(QString);
    QVector<Gaze> getGazesFromSessionAndTarget(QString,QString);
    QVector<std::pair<QString,QString>> getFilesViewed();
    QVector<QVector<QString>> getGazesForSyntacticMapping(QString,bool);
    QVector<QVector<QString>> getGazesForSourceMapping(QString,bool);
    QVector<QString> getFixationRunIDs();
    QVector<QVector<QString>> getFixationsFromRunID(QString);


    void updateGazeWithSyntacticInfo(QString,QString,QString);
    void updateGazeWithTokenInfo(QString,QString,QString);
    QString getUpdateGazeWithSyntacticInfoQuery(QString,QString,QString);
    QString getUpdateGazeWithTokenInfoQuery(QString,QString,QString);

    QVector<QVector<QString>> runFilterQuery(QString);

private:
    QSqlDatabase db;
    QString file_path;
};

#endif // DATABASE_H
