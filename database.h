#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QXmlStreamReader>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QElapsedTimer>
#include <qqml.h>
#include <iostream>
#include <string>



/* Database Class, used to interact between the QSql object and the QML Window
TODO:
    N/A
*/

class Database: public QObject {
public:
    Q_OBJECT

    Q_PROPERTY(QString filePath WRITE openDatabase)

    QML_ELEMENT
public:
    explicit Database(QObject* parent = nullptr);

    void openDatabase(QString);
    void backupDatabase();

    Q_INVOKABLE bool addXMLFile(QString);
    bool addCoreXMLFile(const QString&);
    bool addPluginXMLFile(const QString&);

    bool isDatabaseOpen();
    bool fileExists(const QString&);
    bool participantExists(const QString&);
    bool calibrationExists(const QString&);

private:
    QSqlDatabase db;
    QString path;
};

#endif // DATABASE_H
