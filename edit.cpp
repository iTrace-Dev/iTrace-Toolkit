/********************************************************************************************************************************************************
* @file edit.cpp
*
* @Copyright (C) 2026 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/


#include "edit.h"

Edit::Edit() {}

Edit::Edit(char** argv) {
    edit_id = QString(argv[0]);
    source_file_path = QString(argv[2]);
    text_event_start_timestamp = QString(argv[3]).toLongLong();
    text_event_end_timestamp = QString(argv[4]).toLongLong();
    duration = QString(argv[6]).toInt();
    starting_text = QString(argv[7]);
    ending_text = QString(argv[8]);
    category = QString(argv[9]);
}
