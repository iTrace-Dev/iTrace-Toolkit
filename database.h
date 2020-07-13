#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>      // Might be OK to remove
#include <QApplication> //
#include <QString>
#include <QFile>
#include <QXmlStreamReader>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QDirIterator>
#include <QUuid>
#include <qqml.h>
#include <iostream>
#include <fstream>
//#include <string>
#include <utility>
#include <map>
#include <chrono>
#include <set>
#include <algorithm>
#include "gaze.h"
#include "fixation.h"



/* Database Class, used to interact between the QSql object and the QML Window
TODO:
   - This DESPERATELY needs to be broken up into smaller classes.
     Database has access to way too much
*/

class Database: public QObject {
public:
    Q_OBJECT

    QML_ELEMENT
public:
    explicit Database(QObject* parent = nullptr);

    // Database Functions
    Q_INVOKABLE void createNewDatabase();
    Q_INVOKABLE void openDatabase();
    Q_INVOKABLE void importXML();
    void backupDatabase();

    // XML Functions
    Q_INVOKABLE bool addXMLFile(QString); // change to void?
    Q_INVOKABLE void batchAddXMLFiles();
    bool addCoreXMLFile(const QString&); // change to void?
    bool addPluginXMLFile(const QString&); // change to void?


    // Fixation Functions
    Q_INVOKABLE void generateFixations(); // TODO - allow for custom tasks to be sent

    // Helper Bool Functions
    bool isDatabaseOpen();
    bool fileExists(const QString&);
    bool participantExists(const QString&);
    bool calibrationExists(const QString&);

signals:
    void taskAdded(const QString& sessionID);
    void outputToScreen(const QString& text);

private:
    QSqlDatabase db;
    QString path;
};

#endif // DATABASE_H
