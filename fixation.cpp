#include "fixation.h"

Fixation::Fixation() {}

void Fixation::calculateDatabaseFields() {
    long long start_time = -1, end_time = -1;
    int gaze_count = 0;
    std::map<QString,int> candidate_targets;
    for(auto gaze : gaze_vec) {
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


        left_pupil_diameter += isnan(gaze.left_pupil_diameter) || gaze.left_pupil_diameter == -1.0 ? 0 : gaze.left_pupil_diameter;
        right_pupil_diameter += isnan(gaze.right_pupil_diameter) || gaze.left_pupil_diameter == -1.0 ? 0 : gaze.right_pupil_diameter;

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
    target = fields[0] == "" ? "" : fields[0];
    source_file_line = fields[1] == "" ? -1 : fields[1].toInt();
    source_file_col = fields[2] == "" ? -1 : fields[2].toInt();
    token = fields[3] == "" ? "" : fields[3];
    syntactic_category = fields[4] == "" ? "" : fields[4];
    xpath = fields[5] == "" ? "" : fields[5];

    left_pupil_diameter = left_pupil_diameter / double(gaze_count);
    right_pupil_diameter = right_pupil_diameter / double(gaze_count);
    duration = end_time - start_time;

}

void Fixation::print() {
    std::cout << fixation_event_time << "," << x << "," << y << "," << target.toUtf8().constData() << "," << source_file_line << "," << source_file_col << "," << token.toUtf8().constData() << "," << syntactic_category.toUtf8().constData() << "," << xpath.toUtf8().constData() << "," << left_pupil_diameter << "," << right_pupil_diameter << "," << duration << std::endl;
}
