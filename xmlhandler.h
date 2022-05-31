/********************************************************************************************************************************************************
* @file xmlhandler.h
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

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
