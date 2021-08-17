#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>
#include <ctime>
#include <sys/stat.h>

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
#include "srcmlmapper.h"
#include "logger.h"

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

    //Highlight Functions
    Q_INVOKABLE void highlightFixations(QString,QString);
    void highlightTokens(QVector<QVector<QString>>, SRCMLHandler, QString, QString);
    void generateHighlightedFile(QString,QString,QStringList,QVector<QVector<QString>>);

    //Query Functions
    Q_INVOKABLE QString generateQuery(QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString,QString);
    Q_INVOKABLE void loadQueryFile(QString, QString);
    Q_INVOKABLE void saveQueryFile(QString, QString);
    Q_INVOKABLE void generateQueriedData(QString,QString);


signals:
    void taskAdded(const QString& task);
    void outputToScreen(const QString& color,const QString& msg);
    void warning(const QString& title, const QString& msg);
    void databaseSet(const QString& path);
    void databaseClosed();
    void startProgressBar(int start,int stop);
    void stopProgressBar();
    void setProgressBarValue(int val);
    void setProgressBarToIndeterminate();

private:
    Database idb;
    Logger* log = Logger::instance();
};

#endif // CONTROLLER_H
