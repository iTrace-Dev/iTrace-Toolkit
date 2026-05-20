/********************************************************************************************************************************************************
* @file editalgorithm.h
*
* @Copyright (C) 2026 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#ifndef EDITALGORITHM_H
#define EDITALGORITHM_H

#include "textevent.h"
#include "edit.h"
#include "srcmlhandler.h"
#include <QVector>
#include <QString>
#include <QMap>

class EditAlgorithm {
public:
    EditAlgorithm() {};
    EditAlgorithm(const QVector<TextEvent>& events, const SRCMLHandler& srcml_file, QVector<QString> file_names);
    virtual ~EditAlgorithm() {};

    virtual void generateEdits()=0;
    virtual QString generateEditSettings()=0;

    QVector<Edit> getEdits();
    QString applyTextEvents(QString starting_file_content, const QVector<TextEvent>& events);

protected:

    QVector<TextEvent> text_events;
    QVector<Edit> edits;
    QMap<QString, QString> file_states;
};


class NaiveAlgorithm: public EditAlgorithm {
public:
    NaiveAlgorithm(const QVector<TextEvent>& edits, const SRCMLHandler& srcml_file, QVector<QString> file_names);
    ~NaiveAlgorithm() {}

    void generateEdits() override;
    QString generateEditSettings() override;
};

class DynamicTemporalGapAlgorithm: public EditAlgorithm {
public:
    DynamicTemporalGapAlgorithm(const QVector<TextEvent>& edits, const SRCMLHandler& srcml_file, QVector<QString> file_names, int value);
    ~DynamicTemporalGapAlgorithm() {}

    void generateEdits() override;
    QString generateEditSettings() override;
private:
    int scalar;
};



#endif // EDITALGORITHM_H
