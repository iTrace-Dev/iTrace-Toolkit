#include "editalgorithm.h"
#include "helperfunctions.h"

EditAlgorithm::EditAlgorithm(const QVector<TextEvent>& events, const SRCMLHandler& srcml_file, QVector<QString> file_names) {
    text_events = events;
    // Immediately set the initial state of all the files
    QVector<QString> files = srcml_file.getAllFilenames();
    for (QString filename : files) {
        QString source_text = srcml_file.getEscapedUnitBody(filename);
        QString insert_filename = findMatchingPath(file_names, filename);

        // if we don't have a name from the sessions, then there were no edits and we shouldn't bother
        if (insert_filename == "") {
            continue;
        }

        Edit init_edit;
        init_edit.source_file_path = insert_filename ;
        init_edit.text_event_start_timestamp = 0;
        init_edit.text_event_end_timestamp = 0;
        init_edit.duration = 0;
        init_edit.starting_text = source_text;
        init_edit.ending_text = source_text;
        init_edit.category = "init";

        edits.append(init_edit);
        file_states.insert(filename, source_text);
    }
    
}

QVector<Edit> EditAlgorithm::getEdits() {
    return edits;
}

QString EditAlgorithm::applyTextEvents(QString file_content, const QVector<TextEvent>& events) {

    for (TextEvent event : events) {
        int line = event.source_file_line - 1, col = event.source_file_col - 1;

        QStringList code_lines = file_content.split("\n");
        int file_string_position = 0;
        for (int i = 0; i < line; ++i) {
            QString current_line = code_lines[i];
            file_string_position += current_line.length() + 1;
        }
        file_string_position += col;
//        QString target_line = code_lines[line];

        if (event.deleted_text != "") {
            file_content.remove(file_string_position, event.deleted_text.length());
        }
        if (event.inserted_text != "") {
            file_content.insert(file_string_position, event.inserted_text);
        }

//        code_lines[line] = target_line;
//        file_content = code_lines.join("\n");
    }

    return file_content;
}






//////////////////////
// Naive Algorithm
//////////////////////
NaiveAlgorithm::NaiveAlgorithm(const QVector<TextEvent>& events, const SRCMLHandler& srcml_file, QVector<QString> file_names) : EditAlgorithm(events, srcml_file, file_names) {}

void NaiveAlgorithm::generateEdits() {



    for (TextEvent event : text_events) {

        QVector<QString> all_files = file_states.keys().toVector();

        QString target_file = findMatchingPath(all_files, event.source_file_path);

        QString starting_file_text = file_states[target_file];
        QString updated_file_text = applyTextEvents(starting_file_text, { event });

        Edit edit;
        edit.source_file_path = event.source_file_path;
        edit.text_event_start_timestamp = event.timestamp;
        edit.text_event_end_timestamp = event.timestamp;
        edit.duration = 0;
        edit.starting_text = starting_file_text;
        edit.ending_text = updated_file_text;
        edit.category = "";
        edit.text_event_vec = { event };

        edits.append(edit);
        file_states.insert(event.source_file_path, updated_file_text);
    }


}

QString NaiveAlgorithm::generateEditSettings() {
    return "NAIVE";
}


//////////////////////
// Dynamic Temporal Algorithm
//////////////////////
DynamicTemporalGapAlgorithm::DynamicTemporalGapAlgorithm(const QVector<TextEvent>& events, const SRCMLHandler& srcml_file, QVector<QString> file_names, int value) : EditAlgorithm(events, srcml_file, file_names), scalar(value) {}

void DynamicTemporalGapAlgorithm::generateEdits() {



    // first, calculate the gap between the first text event and the final one
    long long session_length = text_events.last().timestamp - text_events.first().timestamp;

    double pause_threshold = (text_events.size() != 0 ? (double)session_length / text_events.size() : 0) * scalar;

    QVector<TextEvent> event_vec;

    // then go through all text events, group them based on gap and edited file
    for (TextEvent event : text_events) {
        if (event_vec.length() != 0 && (((event.timestamp - event_vec.last().timestamp) > pause_threshold) || (event_vec.last().source_file_path != event.source_file_path))) {

            QVector<QString> all_files = file_states.keys().toVector();

            QString target_file = findMatchingPath(all_files, event_vec.first().source_file_path);
            QString starting_file_text = file_states[target_file];
            QString updated_file_text = applyTextEvents(starting_file_text, event_vec);

            Edit edit;
            edit.source_file_path = event_vec.first().source_file_path;
            edit.text_event_start_timestamp = event_vec.first().timestamp;
            edit.text_event_end_timestamp = event_vec.last().timestamp;
            edit.duration = edit.text_event_end_timestamp - edit.text_event_start_timestamp;
            edit.starting_text = starting_file_text;
            edit.ending_text = updated_file_text;
            edit.text_event_vec = event_vec;

            edits.append(edit);
            event_vec.clear();
            file_states.insert(edit.source_file_path, updated_file_text);
        }

        event_vec.push_back(event);
    }

    // handle the last one
    QVector<QString> all_files = file_states.keys().toVector();
    QString target_file = findMatchingPath(all_files, event_vec.first().source_file_path);
    QString starting_file_text = file_states[target_file];
    QString updated_file_text = applyTextEvents(starting_file_text, event_vec);

    Edit edit;
    edit.source_file_path = event_vec.first().source_file_path;
    edit.text_event_start_timestamp = event_vec.first().timestamp;
    edit.text_event_end_timestamp = event_vec.last().timestamp;
    edit.duration = edit.text_event_end_timestamp - edit.text_event_start_timestamp;
    edit.starting_text = starting_file_text;
    edit.ending_text = updated_file_text;
    edit.text_event_vec = event_vec;

    edits.append(edit);
    file_states.insert(edit.source_file_path, updated_file_text);



}

QString DynamicTemporalGapAlgorithm::generateEditSettings() {
    return "DYNAMIC_TEMPORAL_GAP";
}

