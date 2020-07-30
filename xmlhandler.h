#ifndef XMLHANDLER_H
#define XMLHANDLER_H

#include <QString>
#include <QXmlStreamReader>
#include <QFile>


class XMLHandler {
public:
    XMLHandler();
    XMLHandler(QString);
    ~XMLHandler();

    QString checkAndReturnError();

    QString getXMLFileType();
    QString getNextElementName();
    QString getElementAttribute(QString);

    void resetStream();
    bool isAtEnd();

    QString getFilePath() { return file_path; }
private:
    QString file_path;
    QFile file;
    QXmlStreamReader* xml;
};

#endif // XMLHANDLER_H
