/********************************************************************************************************************************************************
* @file srcmlhandler.h
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#ifndef SRCMLHANDLER_H
#define SRCMLHANDLER_H

#include <QString>
#include <QXmlQuery>
#include <QXmlStreamReader>
#include <QFile>
#include <QTemporaryFile>


class SRCMLHandler {

public:
    SRCMLHandler() {};
    SRCMLHandler(const QString&);
    SRCMLHandler(const QString&, const QString&);

    bool isPositional() const;

    QString getFilePath() const { return file_path; }

    QVector<QString> getAllFilenames() const;

    QString getUnitText(QString) const;
    QString getSoleUnitText() const;
    QString getUnitBody(QString) const;
    QString getSoleUnitBody() const;
    QString getEscapedUnitBody(QString) const;


private:
    QString file_path = "";
    QString raw_srcml_string = "";
    QTemporaryFile temp_file;
};

#endif // SRCMLHANDLER_H
