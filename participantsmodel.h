#ifndef PARTICIPANTSMODEL_H
#define PARTICIPANTSMODEL_H

#include <QAbstractListModel>

class ParticipantsList;

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

    ParticipantsList *getModelList() const;
    void setModelList(ParticipantsList *value);

private:
    ParticipantsList* modelList;
};

#endif // PARTICIPANTSMODEL_H
