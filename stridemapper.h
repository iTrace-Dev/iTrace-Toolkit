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
    void mapSyntax(SRCMLHandler&, QString, QString, bool);
private:
    Database& idb;
};
#endif //STRIDEMAPPER_H
