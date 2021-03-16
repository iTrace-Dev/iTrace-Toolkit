#ifndef XMLHANDLER_H
#define XMLHANDLER_H

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QFile>
#include <QVector>

#include <iostream>


class XMLHandler {
public:
    XMLHandler();
    XMLHandler(QString);
    ~XMLHandler();

    void addString(QString);

    QString checkAndReturnError();

    QString getXMLFileType();
    QString getNextElementName();
    QString getElementAttribute(QString);

    QString getNextElementAsString();

    void resetStream();
    bool isAtEnd();

    QString getFilePath() { return file_path; }
private:
    QString file_path;
    QFile file;
    QXmlStreamReader* xml;

    int i = 0;
};

#endif // XMLHANDLER_H
