#ifndef SRCMLMAPPER_H
#define SRCMLMAPPER_H

#include <iostream>

#include <QApplication>
#include <QString>
#include <QDomDocument>

#include "database.h"
#include "srcmlhandler.h"

class SRCMLMapper
{
public:
    SRCMLMapper(Database& db) : idb(db) {};
    void mapSyntax(SRCMLHandler&, QString, QString, bool);
    void mapToken(SRCMLHandler&, QString, QString, bool);
private:
    Database& idb;
};

#endif // SRCMLMAPPER_H
