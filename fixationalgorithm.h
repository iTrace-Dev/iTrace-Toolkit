#ifndef FIXATIONALGORITHM_H
#define FIXATIONALGORITHM_H

#include "gaze.h"
#include "fixation.h"
#include <vector>

class FixationAlgorithm
{
public:
    FixationAlgorithm() {};

    virtual std::vector<Fixation>& generateFixations()=0;

    std::vector<Fixation>& getFixations();

protected:
    std::vector<Gaze> session_gazes;
    std::vector<Fixation> fixations;
};

#endif // FIXATIONALGORITHM_H
