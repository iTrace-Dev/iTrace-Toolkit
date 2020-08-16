#include "srcmlhandler.h"

SRCMLHandler::SRCMLHandler(QString path) {
    file_path = path;
}

QVector<QString> SRCMLHandler::getAllFilenames() {
    QFile file(file_path);
    file.open(QIODevice::ReadOnly);
    QXmlStreamReader parser(&file);
    QVector<QString> files;

    while(!parser.atEnd()) {
        if(parser.readNextStartElement()) {
            if(parser.name() == "unit") {
                for(auto attr : parser.attributes()) {
                    if(attr.name() == "filename") { files.push_back(attr.value().toString()); }
                }
            }
        }
    }
    file.close();
    return files;
}

QString SRCMLHandler::getUnitText(QString unit_filename) {
    QString srcml_namespace = "declare default element namespace \"http://www.srcML.org/srcML/src\"; declare namespace re=\"http://exslt.org/regular-expressions\";";
    QFile file(file_path);
    file.open(QIODevice::ReadOnly);

    QString query_string = QString("doc($file)//unit[@filename = \""+unit_filename+"\"]");
    QString full_xquery = srcml_namespace + query_string;

    QXmlQuery query_results;
    query_results.bindVariable("file",&file);
    query_results.setQuery(full_xquery);
    if(!query_results.isValid()) { return "INVALID QUERY"; }
    QString results;
    query_results.evaluateTo(&results);
    file.close();
    return results;
}

QString SRCMLHandler::getUnitBody(QString unit_filename) {
    QString srcml_namespace = "declare default element namespace \"http://www.srcML.org/srcML/src\"; declare namespace re=\"http://exslt.org/regular-expressions\";";
    QFile file(file_path);
    file.open(QIODevice::ReadOnly);

    QString query_string = QString("doc($file)//unit[@filename = \""+unit_filename+"\"]/string()");
    QString full_xquery = srcml_namespace + query_string;

    QXmlQuery query_results;
    query_results.bindVariable("file",&file);
    query_results.setQuery(full_xquery);
    if(!query_results.isValid()) { return "INVALID QUERY"; }
    QString results;
    query_results.evaluateTo(&results);
    file.close();
    return results;
}




