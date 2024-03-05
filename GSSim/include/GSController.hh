#ifndef GSCONTROLLER_H
#define GSCONTROLLER_H

#include "TString.h"

class GSController {

public:
    GSController() = default;
    void Get_config(const char* filename);

    int run_num;
    long num_of_fill;
    TString out_dir;
    bool is_uniform_energy;
};

#endif