/********************************************************************************************************************************************************
* @file textevent.h
*
* @Copyright (C) 2026 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#ifndef TEXTEVENT_H
#define TEXTEVENT_H
#include <QVariant>
#include <QSqlQuery>

class TextEvent {
public:
    TextEvent();
    TextEvent(QSqlQuery&);
    TextEvent(char** argv);

    long long timestamp;
    int source_file_line, source_file_col;
    //int session_id;
    QString source_file_path, inserted_text, deleted_text;

};

#endif // TEXTEVENT_H
