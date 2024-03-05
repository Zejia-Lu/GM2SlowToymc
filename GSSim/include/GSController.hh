#ifndef GSCONTROLLER_H
#define GSCONTROLLER_H

#include "TString.h"

class GSController {

public:
    GSController() = default;
    void Get_config(const char* filename);

    int run_num = 0;
    long num_of_fill = 100;
    TString out_dir = "./";
    bool is_uniform_energy = false;

    double lost_rate = 1e-2;
    TString lost_muon_path = "../data/lostmuon_run4all.root";
    TString lost_muon_root_path = "lostMuon/hist_lm";
};

#endif