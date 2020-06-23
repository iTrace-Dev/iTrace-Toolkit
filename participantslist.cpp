#include "participantslist.h"

ParticipantsList::ParticipantsList(QObject *parent) : QObject(parent) {
    nTasks.append({true, "This is a session ID"});
}

QVector<Task> ParticipantsList::items() const {
    return nTasks;
}

bool ParticipantsList::setItemAt(int index, const Task& item) {
    if(index < 0 || index >= nTasks.size())
        return false; // If index is out of bounds return false to notify that nothing has changed

    const Task& oldItem = nTasks.at(index);
    if(item.selected == oldItem.selected && item.sessionID == oldItem.sessionID)
        return false; // If modified item and old item are the same do nothing and return false

    nTasks[index] = item;
    return true;
}

void ParticipantsList::appendTask(const QString& sessionID) {
    emit preItemAppended();

    Task task;
    task.selected = true;
    task.sessionID = sessionID;
    nTasks.append(task);

    emit postItemAppended();
}

void ParticipantsList::removeTask(const QString &sessionId) {
    for(int i = 0; i < nTasks.size();) {
        if(nTasks.at(i).sessionID == sessionId) {
            emit preItemRemoved(i);

            nTasks.removeAt(i);

            emit postItemRemoved();
            return;
        }
        ++i;
    }
}
