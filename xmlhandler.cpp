#include "xmlhandler.h"

XMLHandler::XMLHandler() {}

XMLHandler::XMLHandler(QString fileURL) {
    file_path = fileURL;
    file_path.remove("file:///");

    file.setFileName(file_path); // Open the file to get the raw data
    if(!file.open(QFile::ReadOnly | QFile::Text)) { /* Error will go here */ } // Need to open the file as readonly and text

    xml = new QXmlStreamReader(&file);
}

XMLHandler::~XMLHandler() {
    file.close();
    delete xml;
}

QString XMLHandler::checkAndReturnError() {
    if(xml->hasError()) {
        return xml->errorString();
    }
    return "";
}

// NOTE: Calling this will reset the stream to the beginning
QString XMLHandler::getXMLFileType() {
    xml->readNextStartElement();
    QString rtn = xml->name().toString();
    resetStream();
    return rtn;
}

QString XMLHandler::getNextElementName() {
    while(!xml->readNextStartElement()) {}
    return xml->name().toString();
}

QString XMLHandler::getElementAttribute(QString attr) {
    return xml->attributes().value(attr).toString();
}

void XMLHandler::resetStream() {
    xml->setDevice(xml->device());
}

bool XMLHandler::isAtEnd() {
    return xml->atEnd();
}
