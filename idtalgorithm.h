#ifndef IDTALGORITHM_H
#define IDTALGORITHM_H

#include "fixationalgorithm.h"

class IDTAlgorithm : public FixationAlgorithm{
public:
    IDTAlgorithm(QVector<Gaze> gazes, int _duration, int _dispersion);
    ~IDTAlgorithm() {};

    QVector<Fixation> generateFixations() override;
    QString generateFixationSettings() override;

private:
    Fixation computeFixationEstimate(QVector<Gaze>) override;

    int duration_window;
    int dispersion;
};

#endif // IDTALGORITHM_H
