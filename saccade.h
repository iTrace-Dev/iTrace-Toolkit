#ifndef SACCADE_H
#define SACCADE_H

#include <QString>
#include <vector>
#include <map>
#include <math.h>
#include <gaze.h>
#include <iostream>


//issue 58 - adding the saccade class
class Saccade {
public:
    Saccade();

    void calculateDatabaseFields();

    //void print();

    std::vector<Gaze> gaze_vec;
    double x, y; //left_pupil_diameter = 0, right_pupil_diameter = 0;
    //int source_file_line, source_file_col,
    int duration = 0;
    long long fixation_event_time = 0;
    //QString target = "", syntactic_category = "", token = "", xpath = "";

    int start_x, start_y, end_x, end_y;
    double amplitude, peak_velocity, average_velocity;
    double direction;
    long long start_time=-1;
    long long end_time=-1;
};

#endif // SACCADE_H
