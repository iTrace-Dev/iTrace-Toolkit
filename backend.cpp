#include "backend.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <sstream>
#include <fstream>
#include <QFile>
#include <QFileDialog>


std::string qStrToStr(const QString& qstr) { return qstr.toUtf8().constData(); }

// BackEnd

Backend::Backend(QObject* parent) : QObject(parent) {}

void Backend::outputToConsole(const QString& value) {
    std::cout << value.toUtf8().constData() << std::endl;
}


// FileImporter

FileImporter::FileImporter(QObject* parent) : Backend(parent) {}

void FileImporter::appendFile(const QString& file) {
    files.push_back(file);
    for(QString f : files)
        std::cout << f.toUtf8().constData() << std::endl;
    emit fileImported();
}

void FileImporter::setLabel(const QString& str) { label = str; }

QString FileImporter::getFiles() { // Gets file names (not URLs) from files vector
    std::stringstream ret;
    for(QString f : files) {
        std::string s = f.toUtf8().constData();
        s = s.substr(s.find_last_of('/')+1);
        ret << s << std::endl;
    }
    return QString::fromStdString(ret.str());
}

// FileCreator

FileCreator::FileCreator(QObject* parent) : Backend(parent) {}

void FileCreator::setFileExtension(const QString& ext) { extension = ext; }
/*
void FileCreator::saveFile(const QString& fileName) {
    std::string dir(qStrToStr(QFileDialog::getExistingDirectory(0, "Choose location")) + std::string("/"));
    std::ofstream newDatabase(dir + qStrToStr(fileName+extension));
    std::cout << "Dir: " << dir + qStrToStr(fileName+extension) << std::endl;
    newDatabase.close();
}*/

QString FileCreator::saveFile() {
    QString fileName = QFileDialog::getSaveFileName(nullptr,"Save as","./","SQLite Files (*.db;*.db3;*.sqlite;*.sqlite3);;All Files (*.*)");
    std::cout << "|||" << fileName.toUtf8().constData() << std::endl;
    std::ofstream file(fileName.toUtf8().constData());
    file.close();
    return fileName;
}
