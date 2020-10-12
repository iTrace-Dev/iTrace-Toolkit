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
    
    void startTransaction();
    void commit();
    
    void insertCalibration(QString);
    void insertCalibrationPoint(QString,QString,QString,QString);
    void insertCalibrationSample(QString,QString,QString,QString,QString,QString,QString);
    void insertFile(QString,QString,QString,QString);
    void insertFixation(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString);
    void insertFixationGaze(QString,QString);
    void insertFixationRun(QString,QString,QString,QString);
    void insertGaze(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString);
    void insertIDEContext(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString);
    void insertParticipant(QString,QString);
    void insertSession(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString);
    void insertWebContext(QString,QString,QString,QString,QString);

    QString getSessionFromParticipantAndTask(QString,QString);
    QVector<QString> getSessions();
    QVector<QString> getGazeTargetsFromSession(QString);
    QVector<Gaze> getGazesFromSessionAndTarget(QString,QString);
    QVector<std::pair<QString,QString>> getFilesViewed();
    QVector<QVector<QString>> getGazesForSyntacticMapping(QString,bool);
    QVector<QVector<QString>> getGazesForSourceMapping(QString,bool);
    QVector<QString> getFixationRunIDs();
    QVector<QVector<QString>> getFixationsFromRunID(QString);

    void updateGazeWithSyntacticInfo(QString,QString,QString);
    void updateGazeWithTokenInfo(QString,QString,QString);

private:
    QSqlDatabase db;
    QString file_path;
};

#endif // DATABASE_H
