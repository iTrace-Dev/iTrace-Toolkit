#ifndef BASICALGORITHM_H
#define BASICALGORITHM_H

#include "fixationalgorithm.h"

class BasicAlgorithm: public FixationAlgorithm {
public:
    BasicAlgorithm(QVector<Gaze> gazes, int _window_size, int _radius, int _peak_threshold);

    QVector<Fixation> generateFixations() override;
    Fixation computeFixationEstimate(QVector<Gaze>) override;
    QString generateFixationSettings() override;

private:
    int window_size;
    int radius;
    int peak_threshold;
};

#endif // BASICALGORITHM_H