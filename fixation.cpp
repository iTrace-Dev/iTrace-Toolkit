#include "fixation.h"

Fixation::Fixation() {}

void Fixation::computeFixationEstimate(std::vector<Gaze>& slice) {
    std::vector<float> x_pos, y_pos;
    for(auto gaze : slice) {
        x_pos.push_back(gaze.x);
        y_pos.push_back(gaze.y);
        gaze_vec.push_back(gaze);
    }
    std::sort(x_pos.begin(),x_pos.end());
    std::sort(y_pos.begin(),y_pos.end());
    int median_index = x_pos.size() / 2;
    if(x_pos.size() % 2 == 0) {
        x = (x_pos[median_index - 1] + x_pos[median_index]) / 2;
        y = (y_pos[median_index - 1] + y_pos[median_index]) / 2;
    }
    else {
        x = x_pos[median_index];
        y = y_pos[median_index];
    }
}

void Fixation::calculateDatabaseFields() {
    long long start_time = -1, end_time = -1;
    int gaze_count = 0;
    std::map<QString,int> candidate_targets;
    for(auto gaze : gaze_vec) {
        //std::cout << "x";
        if(!gaze.isValid()) { continue; }
        if(fixation_event_time == 0 || fixation_event_time > gaze.event_time) {
            fixation_event_time = gaze.event_time;
        }

        ++gaze_count;

        if(start_time == -1 || start_time > gaze.system_time) {
            start_time = gaze.system_time;
        }
        if(end_time == -1 || end_time < gaze.system_time) {
            end_time = gaze.system_time;
        }

        left_pupil_diameter += isnan(gaze.left_pupil_diameter) ? 0 : gaze.left_pupil_diameter;
        right_pupil_diameter += isnan(gaze.right_pupil_diameter) ? 0 : gaze.right_pupil_diameter;

        QString candidate_key = gaze.gaze_target + "\t";
        candidate_key += (gaze.source_file_line == -1 ? QString("") : QString::number(gaze.source_file_line)) + "\t";
        candidate_key += (gaze.source_file_col == -1 ? QString("") : QString::number(gaze.source_file_col)) + "\t";
        candidate_key += gaze.source_token + "\t";
        candidate_key += gaze.source_token_syntatic_context + "\t";
        candidate_key += gaze.source_token_xpath + "\t";

        if(candidate_targets.count(candidate_key) == 0) { candidate_targets.insert(std::make_pair(candidate_key,1)); }
        else { ++(candidate_targets.find(candidate_key)->second); }
    }
    std::pair<QString,int> most_frequent = std::make_pair(QString(""),0);
    for(auto candidate : candidate_targets) {
        if(most_frequent.first == "" || most_frequent.second < candidate.second) { most_frequent = candidate; }
    }

    QStringList fields = most_frequent.first.split("\t");
    target = fields[0] == "" ? "null" : fields[0];
    source_file_line = fields[1] == "" ? -1 : fields[1].toInt();
    source_file_col = fields[2] == "" ? -1 : fields[2].toInt();
    token = fields[3] == "" ? "null" : fields[3];
    syntactic_category = fields[4] == "" ? "null" : fields[4];
    xpath = fields[5] == "" ? "null" : fields[5];

    left_pupil_diameter = left_pupil_diameter / float(gaze_count);
    right_pupil_diameter = right_pupil_diameter / float(gaze_count);
    duration = end_time - start_time;

}