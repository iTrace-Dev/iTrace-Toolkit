/********************************************************************************************************************************************************
* @file gaze.cpp
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#include "gaze.h"

Gaze::Gaze() {}

Gaze::Gaze(QSqlQuery& gaze_data) {

    // Core Data
    event_time = gaze_data.value(0).toLongLong();
    x = gaze_data.value(1).toDouble();
    y = gaze_data.value(2).toDouble();
    system_time = gaze_data.value(3).toLongLong();
    left_pupil_diameter = gaze_data.value(4).toDouble();
    right_pupil_diameter = gaze_data.value(5).toDouble();
    left_validation = gaze_data.value(6).toInt();
    right_validation = gaze_data.value(7).toInt();

    // Plugin Data
    gaze_target = gaze_data.value(8).toString();
    gaze_target_type = gaze_data.value(9).toString();
    source_file_line = gaze_data.value(10).toInt();
    source_file_col = gaze_data.value(11).toInt();
    source_token = gaze_data.value(12).toString();
    source_token_xpath = gaze_data.value(13).toString();
    source_token_syntatic_context = gaze_data.value(14).toString();
}//*/

Gaze::Gaze(char** argv) {
    // Core Data
    event_time = QString(argv[0]).toLongLong();
    x = QString(argv[1]).toDouble();
    y = QString(argv[2]).toDouble();
    system_time = QString(argv[3]).toLongLong();
    left_pupil_diameter = QString(argv[4]).toDouble();
    right_pupil_diameter = QString(argv[5]).toDouble();
    left_validation = QString(argv[6]).toInt();
    right_validation = QString(argv[7]).toInt();

    // Plugin Data
    gaze_target = QString(argv[8]);
    gaze_target_type = QString(argv[9]);
    source_file_line = QString(argv[10]).toInt();
    source_file_col = QString(argv[11]).toInt();
    source_token = QString(argv[12]);
    source_token_xpath = QString(argv[13]);
    source_token_syntatic_context = QString(argv[14]);
}

Gaze::Gaze(const Gaze& other) {
    // Core Data
    event_time = other.event_time;
    x = other.x;
    y = other.y;
    system_time = other.system_time;
    left_pupil_diameter = other.left_pupil_diameter;
    right_pupil_diameter = other.right_pupil_diameter;
    left_validation = other.left_validation;
    right_validation = other.right_validation;

    // Plugin Data
    gaze_target = other.gaze_target;
    gaze_target_type = other.gaze_target_type;
    source_file_line = other.source_file_line;
    source_file_col = other.source_file_col;
    source_token = other.source_token;
    source_token_xpath = other.source_token_xpath;
    source_token_syntatic_context = other.source_token_syntatic_context;
}


// Determines if the gaze data is valid. Can be altered later.
bool Gaze::isValid() {
    if(x < 0 || isnan(x)) { return false; }
    if(y < 0 || isnan(y)) { return false; }
    if(left_validation == 0 && right_validation == 0) { return false; }
    return true;
}

void Gaze::print() {
    std::cout << event_time
              << x << y << system_time << left_pupil_diameter
              << right_pupil_diameter << gaze_target.toUtf8().constData()
              << gaze_target_type.toUtf8().constData()
              << source_file_line << source_file_col << source_token.toUtf8().constData()
              << source_token_xpath.toUtf8().constData() << source_token_syntatic_context.toUtf8().constData() << std::endl;

}
