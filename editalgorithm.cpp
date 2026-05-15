#include "editalgorithm.h"
#include "helperfunctions.h"

EditAlgorithm::EditAlgorithm(const QVector<TextEvent>& events, const SRCMLHandler& srcml_file) {
    text_events = events;
    // Immediately set the initial state of all the files
    QVector<QString> files = srcml_file.getAllFilenames();
    for (QString filename : files) {
        QString source_text = srcml_file.getUnitBody(filename);
        Edit init_edit;
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
        QString target_line = code_lines[line];

        if (event.deleted_text != "") {
            target_line.remove(col, event.deleted_text.length());
        }
        if (event.inserted_text != "") {
            target_line.insert(col, event.inserted_text);
        }

        code_lines[line] = target_line;
        file_content = code_lines.join("\n");
    }

    return file_content;
}







NaiveAlgorithm::NaiveAlgorithm(const QVector<TextEvent>& events, const SRCMLHandler& srcml_file) : EditAlgorithm(events, srcml_file) {}

void NaiveAlgorithm::generateEdits() {

    for (TextEvent event : text_events) {

        QVector<QString> all_files = file_states.keys().toVector();

        QString target_file = findMatchingPath(all_files, event.source_file_path);

        QString starting_file_text = file_states[target_file];
        QString updated_file_text = applyTextEvents(starting_file_text, { event });

        Edit edit;
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
