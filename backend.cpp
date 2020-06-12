#include "backend.h"
#include <iostream>

BackEnd::BackEnd(QObject* parent) : QObject(parent) {}


void BackEnd::output(const QString& value) {
    std::cout << value.toUtf8().constData() << std::endl;
}
