#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <qqml.h>
#include <iostream>


class BackEnd : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString value WRITE output)
    QML_ELEMENT

public:
    explicit BackEnd(QObject* parent = nullptr);


    void output(const QString&);

};

#endif // BACKEND_H
