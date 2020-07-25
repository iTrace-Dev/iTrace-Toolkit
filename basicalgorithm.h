#ifndef BASICALGORITHM_H
#define BASICALGORITHM_H

#include "fixationalgorithm.h"

class BasicAlgorithm: public FixationAlgorithm
{
public:
    BasicAlgorithm(int _window_size, int _radius, int _peak_threshold);

    std::vector<Fixation>& generateFixations() override;

private:
    int window_size;
    int radius;
    int peak_threshold;
};

#endif // BASICALGORITHM_H
