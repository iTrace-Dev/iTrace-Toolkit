#include "participantsmodel.h"
#include "participantslist.h"

ParticipantsModel::ParticipantsModel(QObject *parent)
    : QAbstractListModel(parent), modelList(nullptr)
{
}

int ParticipantsModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !modelList)
        return 0;

    return modelList->items().size();
}

QVariant ParticipantsModel::data(const QModelIndex &index, int role) const
{
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

bool ParticipantsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
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

Qt::ItemFlags ParticipantsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> ParticipantsModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[DoneRole] = "done";
    names[DescriptionRole] = "description";
    return names;
}

ParticipantsList *ParticipantsModel::getModelList() const
{
    return modelList;
}

void ParticipantsModel::setModelList(ParticipantsList *list)
{
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
