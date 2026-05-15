#include "textevent.h"

TextEvent::TextEvent() {}

TextEvent::TextEvent(QSqlQuery& text_event_data) {
    timestamp = text_event_data.value(0).toInt();
    source_file_path = text_event_data.value(1).toString();
    source_file_line = text_event_data.value(2).toInt();
    source_file_col = text_event_data.value(3).toInt();
    inserted_text = text_event_data.value(4).toString();
    deleted_text = text_event_data.value(5).toString();
}

TextEvent::TextEvent(char** argv) {
    timestamp = QString(argv[0]).toLongLong();
    source_file_path = QString(argv[1]);
    source_file_line = QString(argv[2]).toInt();
    source_file_col = QString(argv[3]).toInt();
    inserted_text = QString(argv[4]);
    deleted_text = QString(argv[5]);
}
