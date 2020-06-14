#include "backend.h"
#include <iostream>
#include <string>
#include <sstream>

BackEnd::BackEnd(QObject* parent) : QObject(parent) {}


void BackEnd::output(const QString& value) {
    std::cout << value.toUtf8().constData() << std::endl;
}

void BackEnd::appendFile(const QString& file) {
    files.push_back(file);
    for(QString f : files)
        std::cout << f.toUtf8().constData() << std::endl;
    emit fileImported();
}

QString BackEnd::getFiles() { // Gets file names (not URLs) from files vector
    std::stringstream ret;
    for(QString f : files) {
        std::string s = f.toUtf8().constData();
        s = s.substr(s.find_last_of('/')+1);
        ret << s << std::endl;
    }
    return QString::fromStdString(ret.str());
}
