#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>

#include <QObject>
#include <QApplication>
#include <QString>
#include <QVector>
#include <QCryptographicHash>

#include <map>

#include "xmlhandler.h"
#include "database.h"

class Controller : public QObject {
public:
    Q_OBJECT

    //QML_ELEMENT
public:
    explicit Controller(QObject* parent = nullptr);

    //XML Functions
    Q_INVOKABLE void importXMLFile(QString);
    Q_INVOKABLE void batchAddXML(QString);
    void importCoreXML(XMLHandler&);
    void importPluginXML(XMLHandler&);

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
