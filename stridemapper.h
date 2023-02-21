#ifndef STRIDEMAPPER_H
#define STRIDEMAPPER_H

#include <iostream>

#include <QApplication>
#include <QString>
#include <QDomDocument>

#include "database.h"
#include "srcmlhandler.h"

class StrideMapper
{
public:
    StrideMapper(Database& db) : idb(db) {};
    void mapSyntax(QString, QString, bool, QVector<QString>);
    void mapToken(QString, QString, bool, QVector<QString>);
private:
    Database& idb;
};
#endif //STRIDEMAPPER_H
