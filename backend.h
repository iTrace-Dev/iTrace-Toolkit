#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <qqml.h>
#include <iostream>
#include <vector>
#include <string>



// Helper Functions

std::string qStrToStr(const QString&);


/* Parent BackEnd Class, should be used as parent of all other backended classes
TODO:
    N/A
*/
class Backend : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString value WRITE outputToConsole)

    QML_ELEMENT
public:
    explicit Backend(QObject* parent = nullptr);
    void outputToConsole(const QString&);
};


/* FileImporter, used for when the user needs to import the path of certain files.
TODO:
    -Switch from a vector to a more organized system: allowing for tree structure
    -Save path AND name of file
        -Prevent duplicate loading
    -Added label - might not be needed? doesn't hurt for now
*/
class FileImporter : public Backend {
    Q_OBJECT

    Q_PROPERTY(QString fileURL WRITE appendFile NOTIFY fileImported)
    Q_PROPERTY(QString displayLabel WRITE setLabel)

    QML_ELEMENT
public:
    explicit FileImporter(QObject* parent = nullptr);

    void appendFile(const QString&);
    void setLabel(const QString&);

    Q_INVOKABLE QString getFiles();

signals:
    void fileImported();

private:
    QString label;
    std::vector<QString> files;
};


/* FileCreator, used for creating empty files
TODO:
    -When creating file, check for already existing file, and provide confirmation box
*/
class FileCreator : public Backend {
    Q_OBJECT

    Q_PROPERTY(QString fileExtension WRITE setFileExtension)

    QML_ELEMENT
public:
    explicit FileCreator(QObject* parent = nullptr);

    void setFileExtension(const QString&);
    Q_INVOKABLE QString saveFile();
private:
    QString extension;
};

#endif // BACKEND_H
