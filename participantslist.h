#ifndef PARTICIPANTS_H
#define PARTICIPANTS_H

#include <QObject>
#include <QVector>

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

#endif // PARTICIPANTS_H
