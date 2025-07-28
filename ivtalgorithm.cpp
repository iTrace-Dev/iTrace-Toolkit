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

// Define M_PI if it's not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//Helper Functions
double calculateGazeVelocity(double x1, double y1, double x2, double y2) {
    double vx = x1 - x2,
        vy = y1 - y2;
    double v = sqrt((vx*vx)+(vy*vy));
    return v;
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

    //Insert saccades
    QString run_id = "1";
    for (int i = 1; i < fixations.size(); ++i) {
        const Fixation& startFix = fixations[i - 1];
        const Fixation& endFix = fixations[i];

        //const Gaze& origin = startFix.gaze_vec[startFix.gaze_vec.size() - 1];
        //const Gaze& destination = endFix.gaze_vec[0];

        double dx = endFix.x - startFix.x;
        double dy = endFix.y - startFix.y;
        double amplitude = sqrt(dx * dx + dy * dy);

        double direction = atan2(dy, dx) * 180.0 / M_PI;
        if (direction < 0) direction += 360.0;

        qint64 start_time = startFix.gaze_vec.back().system_time;
        qint64 end_time = endFix.gaze_vec.front().system_time;
        double duration = (end_time - start_time);

        double peak_velocity = 0.0;
        double velocity_sum = 0.0;
        int count = 0;
        QVector<QString> gaze_ids;

        for (const Gaze& g : session_gazes) {
            if (g.system_time >= start_time && g.system_time <= end_time) {
                double v = calculateGazeVelocity(startFix.x, startFix.y, g.x, g.y);
                peak_velocity = std::max(peak_velocity, v);
                velocity_sum += v;
                gaze_ids.push_back(QString::number(g.db_id));
                ++count;
            }
        }

        double avg_velocity = (count > 0) ? velocity_sum / count : 0;
        QString saccade_id = QString::number(i); // Replace with actual logic

        QString start_fixation = QString::number(i - 1);
        QString end_fixation = QString::number(i);
        db.insertSaccade(saccade_id,
                         run_id,
                         QString::number(start_time),
                         QString::number(end_time),
                         start_fixation,
                         end_fixation,
                         QString::number(startFix.x),
                         QString::number(startFix.y),
                         QString::number(endFix.x),
                         QString::number(endFix.y),
                         QString::number(amplitude),
                         QString::number(peak_velocity),
                         QString::number(avg_velocity),
                         QString::number(direction),
                         QString::number(duration));

        for (const QString& gid : gaze_ids) {
            db.insertSaccadeGaze(saccade_id, gid);
        }
    }

    return fixations;
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

QString IVTAlgorithm::generateFixationSettings() {
    return "IVT,"+QString::number(velocity_threshold)+","+QString::number(duration_ms);
}
