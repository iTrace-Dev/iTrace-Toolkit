#ifndef PARTICIPANTSMODEL_H
#define PARTICIPANTSMODEL_H

#include <QAbstractListModel>

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

private:
    ParticipantsList* modelList;
};

#endif // PARTICIPANTSMODEL_H
