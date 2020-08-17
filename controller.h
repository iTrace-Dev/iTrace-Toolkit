#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>

#include <qqml.h>
#include <QObject>
#include <QApplication>
#include <QString>
#include <QVector>
#include <QCryptographicHash>
#include <QUuid>
#include <QElapsedTimer>
#include <QDirIterator>
#include <QDomDocument>

#include <map>
#include <fstream>
#include <set>

#include "xmlhandler.h"
#include "database.h"
#include "gaze.h"
#include "fixation.h"
#include "basicalgorithm.h"
#include "idtalgorithm.h"
#include "ivtalgorithm.h"
#include "srcmlhandler.h"

// Algorithm settings enums
enum basic {
    window_size = 1,
    radius,
    peak
};

enum idt {
    duration_window = 1,
    dispersion
};

enum ivt {
    velocity = 1,
    duration
};

class Controller : public QObject {
public:
    Q_OBJECT

    QML_ELEMENT
public:
    explicit Controller(QObject* parent = nullptr);

    //XML Functions
    Q_INVOKABLE void saveDatabaseFile(QString);
    Q_INVOKABLE void loadDatabaseFile(QString);
    Q_INVOKABLE void closeDatabase();
    Q_INVOKABLE void importXMLFile(QString);
    Q_INVOKABLE void batchAddXML(QString);
    void importCoreXML(const QString&);
    void importPluginXML(const QString&);

    //Fixation Functions
    Q_INVOKABLE void generateFixationData(QVector<QString>,QString);

    //srcML Functions
    Q_INVOKABLE void mapTokens(QString,bool);
    QString findMatchingPath(QVector<QString>,QString);
    void mapSyntax(SRCMLHandler&,QString,QString,bool);
    void mapToken(SRCMLHandler&,QString,QString,bool);

signals:
    void taskAdded(const QString& task);
    void outputToScreen(const QString& msg);
    void warning(const QString& title, const QString& msg);
    void databaseSet(const QString& path);
    void databaseClosed();
    void startProgressBar(int start,int stop);
    void stopProgressBar();
    void setProgressBarValue(int val);
    void setProgressBarToIndeterminate();

private:
    Database idb;
};

#endif // CONTROLLER_H
