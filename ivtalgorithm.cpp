/********************************************************************************************************************************************************
* @file ivtalgorithm.cpp
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#include "ivtalgorithm.h"

//Helper Functions
double calculateGazeVelocity(double x1, double y1, double x2, double y2) {
    double vx = x1 - x2,
        vy = y1 - y2;
    double v = sqrt((vx*vx)+(vy*vy));
    return v;
}


double calculateAverageVelocity(QVector<Gaze> gazes ){
    double totalVelocity= 0;
    double averageVelocity=0;
    for (int i=1;i<gazes.size();i++){
        totalVelocity+=calculateGazeVelocity(gazes[i-1].x,gazes[i-1].y, gazes[i].x, gazes[i].y);
    }
    averageVelocity=totalVelocity/gazes.size();
    return averageVelocity;
}

double calculatePeakVelocity(QVector<Gaze> gazes){
    double peakVelocity=0;
    double tmp=0;
    for(int i=1;i<gazes.size();i++){
        tmp=calculateGazeVelocity(gazes[i-1].x,gazes[i-1].y, gazes[i].x, gazes[i].y);
        if(peakVelocity<tmp){peakVelocity=tmp;}
    }
    return peakVelocity;
}


double calculateGazeDirection(QVector <Gaze> gazes){
    double dx=0;
    double dy=0;
    double radians=0;
    double degrees=0;
    for (int i=1;i<gazes.size();i++){
         dx=gazes[i].x-gazes[i-1].x;
         dy=gazes[i].y-gazes[i-1].y;
         radians=atan2(dx,dy);
         degrees=radians*(180/M_PI);
    }
    return degrees;
}

IVTAlgorithm::IVTAlgorithm(QVector<Gaze> gazes, int _velocity, int _duration_ms) : FixationAlgorithm(gazes){
    velocity_threshold = _velocity;
    duration_ms = _duration_ms;
}
QVector<Fixation> IVTAlgorithm::generateFixations() {
    //This code follows the IVT Algorithm

    //Step 1 should already be done

    //Step 2 -  Calculate velocity between each gaze point
    std::vector<double> velocity_vector;
    velocity_vector.push_back(0);

    for(int i = 1; i < session_gazes.size(); ++i) {
        velocity_vector.push_back(calculateGazeVelocity(session_gazes[i-1].x,session_gazes[i-1].y,session_gazes[i].x,session_gazes[i].y));
    }
    //Step 3 - Calculate fixation groupings
    QVector<std::pair<Gaze,int>> fixation_groups;
    int fix_number = 1;
    bool on_saccade = false;

    for(int i = 0; i < session_gazes.size(); ++i) {
        if(velocity_vector[i] <= velocity_threshold) {
            fixation_groups.push_back(std::make_pair(session_gazes[i],fix_number));
            on_saccade = false;
        }
        else if(!on_saccade) {
            on_saccade = true;
            ++fix_number;
        }
    }
    //Step 4 - Filter the fixation groupings
    QVector<Gaze> tmp;
    for(int i = 1; i < fixation_groups.size() - 1; ++i) {
        if(fixation_groups[i].second == fixation_groups[i+1].second) {
            tmp.push_back(fixation_groups[i].first);
        }
        else if(fixation_groups[i].second == fixation_groups[i-1].second) {
            tmp.push_back(fixation_groups[i].first);
            Fixation fix = computeFixationEstimate(tmp);
            if(fix.x > -1) { fixations.push_back(fix); }
            tmp.clear();
        }
        else {
            Fixation fix = computeFixationEstimate(tmp);
            if(fix.x > -1) { fixations.push_back(fix); }
            tmp.clear();
        }
    }
    return fixations;
}

//issue 58 - creating a separate vector for saccades
QVector<Saccade> IVTAlgorithm::generateSaccades() {
    std::vector<double> velocity_vector;
    velocity_vector.push_back(0);

    for(int i = 1; i < session_gazes.size(); ++i) {
        velocity_vector.push_back(calculateGazeVelocity(session_gazes[i-1].x,session_gazes[i-1].y,session_gazes[i].x,session_gazes[i].y));
    }

    //Step 3 - Calculate saccade groupings
    QVector<std::pair<Gaze,int>> saccade_groups;
    int saccade_number = 1;
    bool on_fixation = false;


    for(int i = 0; i < session_gazes.size(); ++i) {
        if(velocity_vector[i] > velocity_threshold) {
            saccade_groups.push_back(std::make_pair(session_gazes[i],saccade_number));
            on_fixation = false;
        }
        else if(!on_fixation) {
            on_fixation = true;
            ++saccade_number;
        }

    }

   // Step 4 - Filter the saccade groupings-needs computeSaccadeEstimate to be implemented
    QVector<Gaze> tmp;

    for(int i = 1; i < saccade_groups.size() - 1; ++i) {
        if(saccade_groups[i].second == saccade_groups[i+1].second) {
            tmp.push_back(saccade_groups[i].first);
        }
        else if(saccade_groups[i].second == saccade_groups[i-1].second) {
            tmp.push_back(saccade_groups[i].first);
            Saccade sacc = computeSaccadeEstimate(tmp);
            if(sacc.start_x>-1) { saccades.push_back(sacc); }
            tmp.clear();
        }
        else {
            Saccade sacc = computeSaccadeEstimate(tmp);
            if(sacc.start_x>-1) { saccades.push_back(sacc); }
            tmp.clear();
        }
    }
    return saccades;
}

Fixation IVTAlgorithm::computeFixationEstimate(QVector<Gaze> fixation_points) {
    Fixation fixation;

    double x_total = 0,
        y_total = 0;
    for(auto point : fixation_points) {
        x_total += point.x;
        y_total += point.y;
        fixation.gaze_vec.push_back(point);
    }
    if(fixation_points.size() <= 1) {
        fixation.x = -1;
        fixation.y = -1;
        return fixation;
    }
    if((fixation_points[fixation_points.size()-1].system_time - fixation_points[0].system_time) >= duration_ms) {
        fixation.x = x_total / fixation_points.size();
        fixation.y = y_total / fixation_points.size();
    }
    else {
        fixation.x = -1;
        fixation.y = -1;
    }
    return fixation;
}

//convert to saccadeEstimate? Will be different from above, will calculate start x and start y
Saccade IVTAlgorithm::computeSaccadeEstimate(QVector<Gaze> saccade_points) {
    Saccade saccade;

    double dx=0;
    double dy=0;

    for(auto point : saccade_points) {
        saccade.start_x=saccade_points.first().x;
        saccade.start_y=saccade_points.first().y;
        saccade.end_x=saccade_points.last().x;
        saccade.end_y=saccade_points.last().y;

        dx=saccade_points.last().x-saccade_points.first().x;
        dy=saccade_points.last().y-saccade_points.first().y;

        saccade.amplitude=sqrt(dx*dx+dy*dy);
        saccade.peak_velocity=calculatePeakVelocity(saccade_points);
        saccade.average_velocity=calculateAverageVelocity(saccade_points);
        saccade.direction=calculateGazeDirection(saccade_points);

        saccade.gaze_vec.push_back(point);
    }
    if(saccade_points.size() < 1) {
        saccade.start_x = -1;
        saccade.start_y = -1;
        saccade.end_x=-1;
        saccade.end_y=-1;
        saccade.average_velocity=-1;
        saccade.amplitude=-1;
        saccade.peak_velocity=-1;
        saccade.direction=-1;
        return saccade;
    }
    //not sure if we'll need the below
    // if((saccade_points[saccade_points.size()-1].system_time - saccade_points[0].system_time) >= duration_ms) {
    //     saccade.start_x = saccade_points.first().x;
    //     saccade.start_y = saccade_points.first().y;
    //     saccade.end_x = saccade_points.last().x;
    //     saccade.end_y = saccade_points.last().y;
    //     saccade.amplitude=sqrt(dx*dx-dy*dy);
    //     saccade.avg_velocity=calculateAverageVelocity(saccade_points);
    //     saccade.peak_velocity=calculatePeakVelocity(saccade_points);
    //return saccade;
    // }
    // else {
    //     saccade.start_x = -1;
    //     saccade.start_y = -1;
    //     saccade.end_x=-1;
    //     saccade.end_y=-1;
    //     saccade.avg_velocity=-1;
    //     saccade.amplitude=-1;
    //     saccade.peak_velocity=-1;
    // }
    return saccade;
}

QString IVTAlgorithm::generateFixationSettings() {
    return "IVT,"+QString::number(velocity_threshold)+","+QString::number(duration_ms);
}
