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

#include <map>
#include <fstream>
#include <set>

#include "xmlhandler.h"
#include "database.h"
#include "gaze.h"
#include "fixation.h"
#include "basicalgorithm.h"


class Controller : public QObject {
public:
    Q_OBJECT

    QML_ELEMENT
public:
    explicit Controller(QObject* parent = nullptr);

    //XML Functions
    Q_INVOKABLE void saveDatabaseFile(QString);
    Q_INVOKABLE void loadDatabaseFile(QString);
    Q_INVOKABLE void importXMLFile(QString);
    Q_INVOKABLE void batchAddXML(QString);
    void importCoreXML(const QString&);
    void importPluginXML(const QString&);

    //Fixation Functions
    Q_INVOKABLE void generateFixationData(QVector<QString>,QString);

signals:
    void taskAdded(const QString& task);
    void outputToScreen(const QString& msg);
    void warning(const QString& title, const QString& msg);

private:
    Database idb;
};

#endif // CONTROLLER_H
