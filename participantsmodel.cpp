#include "participantsmodel.h"
#include "database.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// ParticipantsList
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

///////////////////////////////////////////////////////////////////////////////////////////////////
// ParticipantsModel

ParticipantsModel::ParticipantsModel(QObject *parent) : QAbstractListModel(parent), modelList(nullptr) {
}

int ParticipantsModel::rowCount(const QModelIndex &parent) const {
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !modelList)
        return 0;

    return modelList->items().size();
}

QVariant ParticipantsModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || !modelList)
        return QVariant();

    const Task t = modelList->items().at(index.row());
    switch (role) {
    case DoneRole:
        return QVariant(t.selected);
    case DescriptionRole:
        return QVariant(t.sessionID);
    }
    return QVariant();
}

bool ParticipantsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(!modelList)
        return false;

    Task t = modelList->items().at(index.row());
    switch (role) {
    case DoneRole:
        t.selected = value.toBool();
        break;
    case DescriptionRole:
        t.sessionID = value.toString();
        break;
    }

    if (modelList->setItemAt(index.row(), t)) {
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags ParticipantsModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> ParticipantsModel::roleNames() const {
    QHash<int, QByteArray> names;
    names[DoneRole] = "done";
    names[DescriptionRole] = "description";
    return names;
}

ParticipantsList *ParticipantsModel::getModelList() const {
    return modelList;
}

void ParticipantsModel::setModelList(ParticipantsList *list) {
    beginResetModel(); // Must be called before the source of a model changes

    if(modelList) // If the model is connected to a list already
        modelList->disconnect(this); // we disconnect it from the old list to prepare for the new one

    modelList = list;

    if(modelList) { // If a valid list was set
        connect(modelList, &ParticipantsList::preItemAppended, this, [=]() { // Connects this lambda to the preItemAppendedSignal
            const int index = modelList->items().size();
            beginInsertRows(QModelIndex(), index, index); // 2nd and 3rd parameters tell which indicies to start and end insertion
            // As insert is one beyond the current end of the list, we add to the end of the list
        });

        connect(modelList, &ParticipantsList::postItemAppended, this, [=]() {
            endInsertRows();
        });

        connect(modelList, &ParticipantsList::preItemRemoved, this, [=](int index) {
            beginRemoveRows(QModelIndex(), index, index);
        });

        connect(modelList, &ParticipantsList::postItemRemoved, this, [=]() {
            endRemoveRows();
        });
    }

    endResetModel();
}
