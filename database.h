#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QApplication>
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
#include <qqml.h>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <map>



/* Database Class, used to interact between the QSql object and the QML Window
TODO:
    N/A
*/

class Database: public QObject {
public:
    Q_OBJECT

    Q_PROPERTY(QString filePath WRITE loadDatabase)

    QML_ELEMENT
public:
    explicit Database(QObject* parent = nullptr);

    Q_INVOKABLE void createNewDatabase();
    Q_INVOKABLE void openDatabase();
    Q_INVOKABLE void importXML();
    void loadDatabase(QString);
    void backupDatabase();

    Q_INVOKABLE bool addXMLFile(QString);
    Q_INVOKABLE void batchAddXMLFiles();
    bool addCoreXMLFile(const QString&);
    bool addPluginXMLFile(const QString&);

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
