#ifndef FIXATIONALGORITHM_H
#define FIXATIONALGORITHM_H

#include "gaze.h"
#include "fixation.h"
#include <QVector>

class FixationAlgorithm
{
public:
    FixationAlgorithm() {};
    FixationAlgorithm(QVector<Gaze>& g) { session_gazes = g; }
    virtual ~FixationAlgorithm() {};

    virtual QVector<Fixation> generateFixations()=0;
    virtual QString generateFixationSettings()=0;

    QVector<Fixation>& getFixations();

protected:
    virtual Fixation computeFixationEstimate(QVector<Gaze>)=0;

    QVector<Gaze> session_gazes;
    QVector<Fixation> fixations;
};

#endif // FIXATIONALGORITHM_H
