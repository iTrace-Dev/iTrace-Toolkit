#include "saccade.h"
#include <QDebug>

Saccade::Saccade() {}

void Saccade::calculateDatabaseFields() {
    qDebug() << "DEBUG Saccade: calculateDatabaseFields called";
    qDebug() << "DEBUG Saccade: gaze_vec size =" << gaze_vec.size();

    long long start_time = -1, end_time = -1;
    int gaze_count = 0;
    std::map<QString,int> candidate_targets;

    //qDebug() << "---- Saccade Debug Start ----";
    //qDebug() << "Total gazes in vector:" << gaze_vec.size();

    int index = 0;
    for(auto gaze : gaze_vec) {

        qDebug() << "[Gaze" << index << "]";
        qDebug() << "  Valid:" << gaze.isValid();
        qDebug() << "  system_time:" << gaze.system_time;
        qDebug() << "  event_time:" << gaze.event_time;

        if(!gaze.isValid()) {
            qDebug() << "  -> Skipping invalid gaze";
            ++index;
            continue;
        }

        if(fixation_event_time == 0 || fixation_event_time > gaze.event_time) {
            qDebug() << "  Updating fixation_event_time from"
                     << fixation_event_time << "to" << gaze.event_time;
            fixation_event_time = gaze.event_time;
        }

        ++gaze_count;

        if(start_time == -1 || start_time > gaze.system_time) {
            qDebug() << "  Updating start_time from"
                     << start_time << "to" << gaze.system_time;
            start_time = gaze.system_time;
        }

        if(end_time == -1 || end_time < gaze.system_time) {
            qDebug() << "  Updating end_time from"
                     << end_time << "to" << gaze.system_time;
            end_time = gaze.system_time;
        }

        QString candidate_key = gaze.gaze_target + "\t";
        candidate_key += (gaze.source_file_line == -1 ? QString("") : QString::number(gaze.source_file_line)) + "\t";
        candidate_key += (gaze.source_file_col == -1 ? QString("") : QString::number(gaze.source_file_col)) + "\t";
        candidate_key += gaze.source_token + "\t";
        candidate_key += gaze.source_token_syntatic_context + "\t";
        candidate_key += gaze.source_token_xpath + "\t";

        if(candidate_targets.count(candidate_key) == 0) {
            candidate_targets.emplace(candidate_key,1);
        } else {
            ++(candidate_targets.find(candidate_key)->second);
        }

        ++index;
    }

    //qDebug() << "Valid gaze count:" << gaze_count;
    //qDebug() << "Computed start_time:" << start_time;
    //qDebug() << "Computed end_time:" << end_time;

    std::pair<QString,int> most_frequent = std::make_pair(QString(""),0);
    for(auto candidate = candidate_targets.begin(); candidate != candidate_targets.end(); ++candidate) {
        if(most_frequent.first == "" || most_frequent.second < candidate->second) {
            most_frequent = *candidate;
        }
    }

    duration = end_time - start_time;

    this->start_time = start_time;
    this->end_time = end_time;

    qDebug() << "Final duration:" << duration;
    //qDebug() << "---- Saccade Debug End ----";
}

/*void Saccade::print() {
    std::cout << fixation_event_time << "," << x << "," << y << "," << target.toUtf8().constData() << "," << source_file_line << "," << source_file_col << "," << token.toUtf8().constData() << "," << syntactic_category.toUtf8().constData() << "," << xpath.toUtf8().constData() << "," << left_pupil_diameter << "," << right_pupil_diameter << "," << duration << std::endl;
}*/
