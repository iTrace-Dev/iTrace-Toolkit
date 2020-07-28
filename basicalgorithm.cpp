#include "basicalgorithm.h"

BasicAlgorithm::BasicAlgorithm(QVector<Gaze> gazes, int _window_size, int _radius, int _peak_threshold) : FixationAlgorithm(gazes) {
    window_size = _window_size;
    radius = _radius;
    peak_threshold = _peak_threshold;
}

QVector<Fixation> BasicAlgorithm::generateFixations() {
    //This code follows the Olsson Algorithm

    //Step 1 should already be done

    //Step 2 - Calculate a vector of differences between each gaze
    std::vector<double> differences;
    for(int i = window_size; i < int(session_gazes.size()) + 1 - window_size; ++i) {
        std::pair<double,double> before = {0.0,0.0},
                               after =  {0.0,0.0};
        for(int j = 0; j < window_size; ++j) {
            before.first += session_gazes[i - (j + 1)].x;
            before.second += session_gazes[i - (j + 1)].y;
            after.first += session_gazes[i + j].x;
            after.second += session_gazes[i + j].y;
        }
        before.first /= window_size;
        before.second /= window_size;
        after.first /= window_size;
        after.second /= window_size;
        differences.push_back(sqrt(pow(after.first - before.first,2) + pow(after.second - before.second,2)));
    }

    //Step 3-5 - Find the indicies of the Peaks
    //step 3
    std::vector<double> peaks(differences.size());
    for(int i = 0; i < int(differences.size()); ++i) { peaks[i] = 0.0; }
    for(int i = 1; i < int(differences.size()) - 1; ++i) {
        if(differences[i] > differences[i-1] && differences[i] > differences[i+1]) {
            peaks[i] = differences[i];
        }
    }
    //step 4
    for(int i = window_size-1; i < int(peaks.size()); ++i) {
        int start = i - (window_size - 1),
            end = i;
        while(start != end) {
            if(peaks[start] >= peaks[end]) {
                peaks[end] = 0.0;
                --end;
            }
            else {
                peaks[start] = 0.0;
                ++start;
            }
        }
    }
    //step 5
    std::vector<int> indicies;
    for(int i = 0; i < int(peaks.size()); ++i) {
        if(peaks[i] >= peak_threshold) {
            indicies.push_back(i);
        }
    }

    //Step 6? - Calculate the spaitial fixations through estimation
    double shortest_dis = 0;
    //QVector<Fixation> fixations;

    while(shortest_dis < radius) {

        fixations.clear();
        int start_peak_index = 0;

        for(auto index : indicies) {
            std::vector<Gaze> slice;
            auto start = session_gazes.begin() + start_peak_index;
            auto end = session_gazes.begin() + index;
            copy(start, end, std::back_inserter(slice));
            //computerFixationEstimate
            Fixation fix = Fixation();
            fix.computeFixationEstimate(slice);
            //end computerFixationEstimate
            fixations.push_back(fix);
            start_peak_index = index;
        }
        Fixation fix = Fixation();
        std::vector<Gaze> slice;
        copy(session_gazes.begin() + start_peak_index, session_gazes.end(), std::back_inserter(slice));
        fix.computeFixationEstimate(slice);
        fixations.push_back(fix);

        shortest_dis = INFINITY;
        Fixation* previous_estimate = nullptr;
        int peak_index = -1, peak_removal_index = -1;
        auto crnt = fixations.begin();
        for(; crnt != fixations.end(); ++crnt) {
            if(previous_estimate != nullptr) {
                double distance = sqrt(pow((*crnt).x - (*previous_estimate).x,2) + pow((*crnt).y - (*previous_estimate).y,2));
                if(distance < shortest_dis) {
                    shortest_dis = distance;
                    peak_removal_index = peak_index;
                }
            }
            previous_estimate = &*crnt;
            ++peak_removal_index;
        }
        if(shortest_dis < radius) { indicies.erase(indicies.begin() + peak_removal_index); }
    }
    return fixations;
}

QString BasicAlgorithm::generateFixationSettings() {
    return "BASIC," + QString::number(window_size) + "," + QString::number(radius) + "," + QString::number(peak_threshold);
}
