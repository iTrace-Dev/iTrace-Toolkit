#include "xmlhandler.h"

XMLHandler::XMLHandler() { xml = new QXmlStreamReader(); }

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

void XMLHandler::addString(QString xml_data) {
    xml->addData(xml_data);
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
    //resetStream();
    return rtn;
}

QString XMLHandler::getNextElementName() {
    //while(!xml->readNextStartElement()) {}
    while(true) {
        auto x = xml->readNext();
        if(x == QXmlStreamReader::StartElement) { break; }
        if(x == QXmlStreamReader::Invalid) { break; }
    }

    return xml->name().toString();
}

QString XMLHandler::getElementAttribute(QString attr) {
    return xml->attributes().value(attr).toString();
}

QString XMLHandler::getNextElementAsString() {
    //if(i >= 1000) { std::cout << i << std::endl; }
    while(!isAtEnd()) {
        auto x = xml->readNext();
        if(x == QXmlStreamReader::StartElement) { break; }
        if(x == QXmlStreamReader::Invalid) { break; }
    }
    QString name = xml->name().toString();
    QString rtn = "<" + name + " ";



    for(auto attr : xml->attributes()) {
        rtn += attr.name() + "=\"" + attr.value() + "\" ";
    }
    xml->readNext();
    QString text = xml->text().toString().trimmed();
    rtn += ">" + text + "</" + name + ">";
    ++i;
    return rtn;
}

void XMLHandler::resetStream() {
    xml->setDevice(xml->device());
}

bool XMLHandler::isAtEnd() {
    return xml->atEnd();
}
