#include "stridemapper.h"

void StrideMapper::mapSyntax(QString unit_path, QString project_path, bool overwrite) {
    QVector<QVector<QString>> responses = idb.getGazesForSyntacticMapping(project_path,overwrite);

    QDomDocument unit;
    QFile file(unit_path);
    file.open(QIODevice::ReadOnly);
    unit.setContent(&file, false);

    QVector<QDomElement> elements;
    elements.append(unit.documentElement());
    QDomElement first = unit.documentElement().firstChildElement();

    QVector<QDomElement> parents; parents.push_back(first);
    while(parents.size() != 0) {
        QDomElement crnt = parents[parents.size() - 1];

        elements.push_back(crnt);

        if(!crnt.firstChildElement().isNull()) {
            parents.push_back(crnt.firstChildElement());
        }
        else if(!crnt.nextSiblingElement().isNull()) {
            parents[parents.size() - 1] = crnt.nextSiblingElement();
        }
        else {
            while(parents.size() != 0 && parents[parents.size() - 1].nextSiblingElement().isNull()) {
                parents.pop_back();
            }
            if(parents.size() != 0) { parents[parents.size() - 1] = parents[parents.size() - 1].nextSiblingElement(); }
        }
        QApplication::processEvents();
    }

    // Each response is an entry in the list of gazes from BlueJ:
    std::map<QString,std::pair<QString,QString>> cached_gazes;
    int i = -1;
    for(auto response : responses) {
        ++i;

        //THIS CAN CHANGE IN THE FUTURE
        QString gaze_key = project_path + "L" + response[1] + "C" + response[2];

        if(cached_gazes.count(gaze_key) > 0) {
            idb.updateGazeWithSyntacticInfo(response[0],cached_gazes.at(gaze_key).first,cached_gazes.at(gaze_key).second);
            continue;
        }

        QString syntactic_context = "",
                xpath = response[5];
        cached_gazes.emplace(gaze_key,std::make_pair(xpath,syntactic_context));
        idb.updateGazeWithSyntacticInfo(response[0],xpath,syntactic_context);
    }
}

// Decode underscores, e.g. "else_32if" becomes "else if"
QString decode_underscores(const QString& unprocessed)
{
    // This is a bit of a hack, but the only non-ASCII char is the assignment, so handle that specially:
    QString original = QString(unprocessed).replace("_8656", "\u21D0");
    // Otherwise, find all underscores and convert the two decimal digits after:
    QString r = "";
    for (int i = 0; i < original.length(); i++) {
        if (original.at(i).toLatin1() == '_') {
            r.append(QChar((original.at(i + 1).toLatin1() - '0') * 10 + (original.at(i + 2).toLatin1() - '0')));
            i += 2;
        }
        else {
            r.append(original.at(i));
        }
    }
    return r;
}

// Get rid of trailing square bracket index, e.g. "assign[2]" becomes "assign"
QString strip_square_number(const QString& original) {
    int i = original.indexOf('[');
    if (i >= 0)
        return original.left(i);
    else
        return original;
}

// Given an XPath specifying an attribute, run it against the given file
QString fetch_attribute(const QString& xpath, QFile* file)
{
    QXmlQuery query;
    file->seek(0);
    query.setFocus(file);
    // Due to a bug in the Stride recording, it references e.g. @src, but the position is relative to @src-java
    // Qt is strict about its XPath so we must append /string() to get the text out:
    query.setQuery(xpath + "-java/string()");
    QString r;
    query.evaluateTo(&r);
    // Qt always puts \n on the end of the result, so we must remove that:
    return r.trimmed();
}

QRegExp digits("[0-9]+");
QRegExp nonDigit("[^0-9]");
// The non-identifier rules in Java are complex, this is only for ASCII:
QRegExp nonIdentifierPart("[^a-zA-Z0-9_]");
QStringList symbols = QString("||= &&= >>> << >> ++ -- += -= *= /= |= &= <= >= == != + - * / ! % & ^ | ~ < > = ? : , . [ ] ( ) { }").split(" ");

// Assume the start of a string is a token.  What's the start of the next token after that?
int start_of_next_token(const QString& expression)
{
    if (expression.isEmpty())
        return 0;
    // Skip past space:
    if (expression.at(0).isSpace())
        return 1 + start_of_next_token(expression.mid(1));

    // Look out for any operators or symbols.  Must check longest ones first:

    for (const QString& symbol : symbols) {
        // This will treat - before a number as a unary minus, but I think that's ok:
        if (expression.startsWith(symbol))
            return symbol.length();
    }
    // Not a symbol, could be a string or character literal:
    if (expression.startsWith("\"") || expression.startsWith("\'")) {
        // Find the next expression.at(0) not preceded by a \
        // (This isn't fully accurate e.g. if it ends \\" but it will do for our study)
        for (int next_quote = expression.indexOf(expression.at(0), 1); next_quote >= 0; next_quote = expression.indexOf(expression.at(0), next_quote))
        {
            if (expression.at(next_quote - 1).toLatin1() != '\\') {
                return next_quote + 1;
            }
        }
        // Couldn't find the end, maybe unterminated so just return this whole expression then:
        return expression.length();
    }
    // Could be a number literal (we don't support hex at the moment):
    else if (digits.exactMatch(expression.left(1))) {
        // Find the first non-digit:
        int next = expression.indexOf(nonDigit);
        if (next >= 0 && expression.at(next).toLatin1() == '.') {
            // Look for more digits:
            next = expression.indexOf(nonDigit, next + 1);
        }
        if (next < 0)
            return expression.length();
        else
            return next;
    }
    // Otherwise it's an identifier (including keywords like true, false, null).
    // Find the first non-identifier character:
    else {
        int next = expression.indexOf(nonIdentifierPart, 1);
        if (next < 0)
            return expression.length();
        else
            return next;
    }
}

// Given a String with a Stride expression, get the token that includes the character at the given index
QString token_at(const QString& whole_expression, int char_index)
{
    QString cur(whole_expression);
    // Our default fallback is the whole string, otherwise it's the previous token:
    QString fallback = cur.trimmed();
    while (true) {
        int start_next = start_of_next_token(cur);
        // If we don't find the end or we're at the end, return our fallback:
        if (start_next <= 0)
            return fallback.trimmed();
        if (char_index <= start_next)
            return cur.left(start_next).trimmed();
        // Adjust our search target and remove the token from the string:
        fallback = cur.left(start_next);
        cur.remove(0, start_next);
        char_index -= start_next;
    }
}

void StrideMapper::mapToken(QString unit_path, QString project_path, bool overwrite) {
    QVector<QVector<QString>> responses = idb.getGazesForSourceMapping(project_path,overwrite);

    QDomDocument unit;
    QFile file(unit_path);
    file.open(QIODevice::ReadOnly);
    unit.setContent(&file, false);

    QVector<QDomElement> elements;
    elements.append(unit.documentElement());
    QDomElement first = unit.documentElement().firstChildElement();

    QVector<QDomElement> parents; parents.push_back(first);
    while(parents.size() != 0) {
        QDomElement crnt = parents[parents.size() - 1];

        elements.push_back(crnt);

        if(!crnt.firstChildElement().isNull()) {
            parents.push_back(crnt.firstChildElement());
        }
        else if(!crnt.nextSiblingElement().isNull()) {
            parents[parents.size() - 1] = crnt.nextSiblingElement();
        }
        else {
            while(parents.size() != 0 && parents[parents.size() - 1].nextSiblingElement().isNull()) {
                parents.pop_back();
            }
            if(parents.size() != 0) { parents[parents.size() - 1] = parents[parents.size() - 1].nextSiblingElement(); }
        }
        QApplication::processEvents();
    }

    // Each response is an entry in the list of gazes from BlueJ:
    std::map<QString,std::pair<QString,QString>> cached_gazes;
    int i = -1;
    for(auto response : responses) {
        ++i;
        QString xpath = response[5];
        QString gaze_key = xpath;
        if(cached_gazes.count(gaze_key) > 0) {
            idb.updateGazeWithTokenInfo(response[0],cached_gazes.at(gaze_key).first,cached_gazes.at(gaze_key).second);
            continue;
        }


        QString token = "";
        QString token_type = "";
        if (xpath.startsWith("substring")) {
            // It's a substring of an attribute value, so fetch it and substring it:
            // Scrap the substring() part and split by commas:
            QStringList params = xpath.mid(QString("substring").length()).replace('(',"").replace(')',"").split(", ");
            QString attr = fetch_attribute(params.value(0), &file);
            token = token_at(attr, params.value(1).toInt());
            std::cout << "Found token " << token.toStdString() << " at " << attr.toStdString() << "@" << params.value(1).toStdString() << std::endl;
            token_type = "expression";
        }
        else {
            QStringList pieces = xpath.split("/");
            QString last = pieces.value( pieces.length() - 1 );
            if (last.endsWith("]")) {
                // It's a numbered item like method[1], so we strip the number and use that as the type, with a token of ""
                token = "";
                token_type = strip_square_number(last) + "_background";
            }
            else if (last.startsWith("_")) {
                token = decode_underscores(last.mid(1));
                token_type = strip_square_number(pieces.value(pieces.length() - 2)) + "_subtoken";
            }
            else if (last.startsWith("@")) {
                // It's an XML attribute, which we must fetch from source:
                token = fetch_attribute(xpath, &file);
                token_type = strip_square_number(pieces.value(pieces.length() - 2)) + "_part";
            }
        }

        // TEMP:
        std::cout << "At " << i << " mapped " << xpath.toStdString() << " to " << token.toStdString() << ";" << token_type.toStdString() << std::endl;
        cached_gazes.insert(std::make_pair(gaze_key,std::make_pair(token,token_type)));
        idb.updateGazeWithTokenInfo(response[0],token,token_type);
    }
}
