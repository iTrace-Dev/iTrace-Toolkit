/********************************************************************************************************************************************************
* @file participantsmodel.h
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#ifndef PARTICIPANTSMODEL_H
#define PARTICIPANTSMODEL_H

#include <QAbstractListModel>

#include "logger.h"

struct Task {
    bool selected;
    QString sessionID;
};

class ParticipantsList : public QObject
{
    Q_OBJECT

public:
    explicit ParticipantsList(QObject *parent = nullptr);

    QVector<Task> items() const;

    Q_INVOKABLE QVector<QString> getSelected() const;

    bool setItemAt(int index, const Task& item);

    void clearTasks();


signals:
    void preItemAppended();
    void postItemAppended();

    void preItemRemoved(int);
    void postItemRemoved();

public slots:
    void appendTask(const QString& sessionId);
    void removeTask(const QString& sessionId);

private:
    QVector<Task> nTasks;

};

class ParticipantsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ParticipantsList* list READ getModelList WRITE setModelList)

public:
    explicit ParticipantsModel(QObject *parent = nullptr);

    enum {
        DoneRole = Qt::UserRole,
        DescriptionRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE ParticipantsList* getModelList() const;
    void setModelList(ParticipantsList *value);

    Q_INVOKABLE void appendTask(const QString& sessionID) {modelList->appendTask(sessionID);}

    Q_INVOKABLE void clearTasks() {
        modelList->clearTasks();
        //setModelList(modelList); // Old code that caused a Heap error
        setModelList(new ParticipantsList());
    }

private:
    ParticipantsList* modelList;
    Logger* log = Logger::instance();
};

#endif // PARTICIPANTSMODEL_H
