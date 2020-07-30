#ifndef IDTALGORITHM_H
#define IDTALGORITHM_H

#include "fixationalgorithm.h"

class IDTAlgorithm : public FixationAlgorithm{
public:
    IDTAlgorithm(QVector<Gaze> gazes, int _duration, int _dispersion);

    QVector<Fixation> generateFixations() override;
    Fixation computeFixationEstimate(QVector<Gaze>) override;
    QString generateFixationSettings() override;

private:
    int duration_window;
    int dispersion;
};

#endif // IDTALGORITHM_H
