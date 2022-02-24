#ifndef SRCMLHANDLER_H
#define SRCMLHANDLER_H

#include <QString>
#include <QXmlQuery>
#include <QXmlStreamReader>
#include <QFile>


class SRCMLHandler {

public:
    SRCMLHandler() {};
    SRCMLHandler(QString);

    QVector<QString> getAllFilenames();

    QString getUnitText(QString);
    QString getUnitBody(QString);


private:
    QString file_path;
};

#endif // SRCMLHANDLER_H
