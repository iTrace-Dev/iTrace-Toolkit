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
