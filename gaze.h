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

    bool isValid();

    void print();

    int left_validation, right_validation,
        source_file_line = -1, source_file_col = -1;
    long long event_time, system_time;
    double x, y, left_pupil_diameter, right_pupil_diameter;
    QString gaze_target = "", gaze_target_type = "",
                source_token = "", source_token_xpath = "", source_token_syntatic_context = "";

};

#endif // GAZE_H
