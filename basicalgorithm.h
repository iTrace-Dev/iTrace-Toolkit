/********************************************************************************************************************************************************
* @file basicalgorithm.h
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#ifndef BASICALGORITHM_H
#define BASICALGORITHM_H

#include "fixationalgorithm.h"

class BasicAlgorithm: public FixationAlgorithm {
public:
    BasicAlgorithm(QVector<Gaze> gazes, int _window_size, int _radius, int _peak_threshold);
    ~BasicAlgorithm() {};


    QVector<Fixation> generateFixations() override;

    QString generateFixationSettings() override;

private:
    Fixation computeFixationEstimate(QVector<Gaze>) override;

    int window_size;
    int radius;
    int peak_threshold;
};

#endif // BASICALGORITHM_H
