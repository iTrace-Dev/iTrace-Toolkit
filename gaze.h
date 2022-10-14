/********************************************************************************************************************************************************
* @file gaze.h
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#ifndef GAZE_H
#define GAZE_H
#include <string>
#include <QSqlQuery>
#include <QVariant>
#include <iostream>
#include <math.h>


class Gaze {
public:
    Gaze();
    Gaze(QSqlQuery&);
    Gaze(char**);
    Gaze(const Gaze&);

    bool isValid();

    void print();

    int left_validation, right_validation,
        source_file_line = -1, source_file_col = -1;
    long long event_time, system_time;
    double x, y, left_pupil_diameter, right_pupil_diameter;
    QString gaze_target = "", gaze_target_type = "",
                source_token = "", source_token_xpath = "", source_token_syntatic_context = "";

    //friend std::ostream& operator<<(std::ostream&,const Gaze&);

};

#endif // GAZE_H
