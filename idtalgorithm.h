/********************************************************************************************************************************************************
* @file idtalgorithm.h
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#ifndef IDTALGORITHM_H
#define IDTALGORITHM_H

#include "fixationalgorithm.h"

class IDTAlgorithm : public FixationAlgorithm{
public:
    IDTAlgorithm(QVector<Gaze> gazes, int _duration, int _dispersion, int _max_gaze_span);
    ~IDTAlgorithm() {};

    QVector<Fixation> generateFixations() override;
    QString generateFixationSettings() override;

private:
    Fixation computeFixationEstimate(QVector<Gaze>) override;

    int duration_window;
    int dispersion;
    int max_gaze_span;
};

#endif // IDTALGORITHM_H
