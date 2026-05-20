/********************************************************************************************************************************************************
* @file edit.h
*
* @Copyright (C) 2026 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/


#ifndef EDIT_H
#define EDIT_H

#include "textevent.h"
#include <QString>
#include <QVector>

class Edit
{
public:
    Edit();
    Edit(char**);

    QVector<TextEvent> text_event_vec;
    long long text_event_start_timestamp, text_event_end_timestamp;
    int duration = 0;
    QString source_file_path = "", starting_text = "", ending_text = "", category = "";

    // only needed during token mapping
    QString edit_id = "";

};

#endif // EDIT_H
