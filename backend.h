#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <qqml.h>
#include <iostream>
#include <vector>


class BackEnd : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString value WRITE output)

    Q_PROPERTY(QString fileURL WRITE appendFile NOTIFY fileImported)


    QML_ELEMENT

public:
    explicit BackEnd(QObject* parent = nullptr);

    void output(const QString&);
    void appendFile(const QString&);

    Q_INVOKABLE QString getFiles();

signals:
    void fileImported();

private:
    std::vector<QString> files;

};

#endif // BACKEND_H
