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
