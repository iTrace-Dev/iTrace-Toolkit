/********************************************************************************************************************************************************
* @file idtalgorithm.cpp
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#include "idtalgorithm.h"

//Helper Functions
bool compareX(Gaze i, Gaze j) { return i.x < j.x; }
bool compareY(Gaze i, Gaze j) { return i.y < j.y; }
double computeGazeDifference(QVector<Gaze> gazes) {
    double xmax = std::max_element(gazes.begin(),gazes.end(),compareX)->x,
           xmin = std::min_element(gazes.begin(),gazes.end(),compareX)->x,
           ymax = std::max_element(gazes.begin(),gazes.end(),compareY)->y,
           ymin = std::min_element(gazes.begin(),gazes.end(),compareY)->y;
    double final = (xmax - xmin) + (ymax - ymin);
    return final;
}

IDTAlgorithm::IDTAlgorithm(QVector<Gaze> gazes, int _duration, int _dispersion) : FixationAlgorithm(gazes) {
    duration_window = _duration;
    dispersion = _dispersion;
}

QVector<Fixation> IDTAlgorithm::generateFixations() {

    //This code follows the IDT Algorithm

    //Step 1 should already be done

    //Step 2 - Calculate velocity between each point and generate Fixations
    QVector<Gaze> window;
    int i = 0;

    while(i < duration_window && i < session_gazes.size()) {
        window.push_back(session_gazes[i]);
        ++i;
    }

    while(i < session_gazes.size()) {
        if((computeGazeDifference(window) <= dispersion) && (window.size() >= duration_window)) {
            while(computeGazeDifference(window) <= dispersion) {
                if(i < session_gazes.size() - 1) {
                    window.push_back(session_gazes[i]);
                    ++i;
                }
                else { break; }
            }
            fixations.push_back(computeFixationEstimate(window));
            window.clear();
            window.push_back(session_gazes[i]);
            ++i;
        }
        else if(window.size() < duration_window) {
            window.push_back(session_gazes[i]);
            ++i;
        }
        else {
            window.pop_front();
            if(window.size() < duration_window) {
                window.push_back(session_gazes[i]);
                ++i;
            }
        }
    }

    return fixations;
}

Fixation IDTAlgorithm::computeFixationEstimate(QVector<Gaze> fixation_points) {
    Fixation fixation;
    double x_total = 0, y_total = 0;
    for(auto point : fixation_points) {
        x_total += point.x;
        y_total += point.y;
        fixation.gaze_vec.push_back(point);
    }
    fixation.x = x_total / fixation_points.size();
    fixation.y = y_total / fixation_points.size();

    return fixation;
}

QString IDTAlgorithm::generateFixationSettings() {
    return "IDT," + QString::number(dispersion) + "," + QString::number(duration_window);
}

