#ifndef FIXATION_H
#define FIXATION_H

#include <QString>
#include <vector>
#include <map>
#include <math.h>
#include <gaze.h>
#include <iostream>

class Fixation {
public:
    Fixation();
    void computeFixationEstimate(std::vector<Gaze>&);
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
