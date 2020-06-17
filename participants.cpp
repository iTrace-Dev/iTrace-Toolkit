#include "participants.h"

Participants::Participants(QObject *parent) : QObject(parent)
{

}

QVector<Task> Participants::items() const
{
    return nTasks;
}

bool Participants::setItemAt(int index, const Task& item)
{
    if(index < 0 || index >= nTasks.size())
        return false; // If index is out of bounds return false to notify that nothing has changed

    const Task& oldItem = nTasks.at(index);
    if(item.selected == oldItem.selected && item.name == oldItem.name)
        return false; // If modified item and old item are the same do nothing and return false

    nTasks[index] = item;
    return true;
}

void Participants::appendTask(/*const QString& session*/)
{
    emit preItemAppended();

    Task item;
    item.selected = true;
    item.name = "Session ID"; // session;

    emit postItemAppended();

}
