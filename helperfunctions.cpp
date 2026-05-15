/********************************************************************************************************************************************************
* @file helperfunctions.cpp
*
* @Copyright (C) 2026 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#include "helperfunctions.h"

#include <QString>
#include <QStringList>

// This should probably be a helper function
QString findMatchingPath(QVector<QString> all_files, QString file) {
    file.replace("\\","/");
    file = file.toLower();
    QVector<QStringList> possible;
    QStringList file_split = file.split("/");
    QString check = file_split[file_split.size()-1];
    for(auto i : all_files) {
        if(i.toLower().endsWith(check)) { possible.push_back(i.split("/")); }
    }
    if(possible.size() == 0) { return ""; }
    else if(possible.size() == 1) { return possible[0].join("/"); }

    QString shortest = "";
    int passes = 1;


    while(possible.size() != 1) {
        QVector<QStringList> candidates;
        if(passes > file_split.size()) { return shortest; }
        for(auto unit_path : possible) {
            if(passes > unit_path.size()) {
                if(shortest == "") { shortest = unit_path.join("/"); }
                continue;
            }
            QString unit_check = unit_path[unit_path.size() - passes].toLower();
            QString file_check = file_split[file_split.size() - passes];
            if(unit_check == file_check) {
                candidates.push_back(unit_path);
            }
        }
        possible = candidates;
        ++passes;
    }
    if(possible.size() == 0) { return ""; }
    return possible[0].join("/");
}
