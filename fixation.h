#ifndef FIXATION_H
#define FIXATION_H

#include <QString>
#include <vector>
#include <map>
#include <math.h>
#include <gaze.h>
#include <iostream>

/*//////////////////////////////////////
gaze_vec - vector of gazes that go with this fixation
x, y - ? x and y coordinates of the fixation on the screen ?
left_pupil_diameter, right_pupil_diameter - average value of the user's pupil diameters. In millimeters
source_file_line, source_file_column - line and column position of the token of the fixation
duration - time in milliseconds of how long the fixation lasted
fixation_event_time - unix time of when the fixation occurred
target - file that the fixation occurred in
syntactic_category - category of syntax the token belongs to
token - what the fixation was looking at
xpath - the XML xpath to get the token
//////////////////////////////////////*/

class Fixation {
public:
    Fixation();

    void calculateDatabaseFields();

    void print();

    // This could maybe be moved to a set if we want to ignore duplicates
    std::vector<Gaze> gaze_vec;
    double x, y, left_pupil_diameter = 0, right_pupil_diameter = 0;
    int source_file_line, source_file_col, duration = 0;
    long long fixation_event_time = 0;
    QString target = "", syntactic_category = "", token = "", xpath = "";
};

#endif // FIXATION_H
