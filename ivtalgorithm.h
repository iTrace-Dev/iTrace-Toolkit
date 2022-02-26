/********************************************************************************************************************************************************
* @file ivtalgorithm.h
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#ifndef IVTALGORITHM_H
#define IVTALGORITHM_H

#include "fixationalgorithm.h"

class IVTAlgorithm: public FixationAlgorithm {
public:
    IVTAlgorithm(QVector<Gaze> gazes, int _velocity, int _duration_ms);
    ~IVTAlgorithm() {};

    QVector<Fixation> generateFixations() override;
    QString generateFixationSettings() override;
private:
    Fixation computeFixationEstimate(QVector<Gaze>) override;

    int velocity_threshold;
    int duration_ms;
};

#endif // IVTALGORITHM_H
