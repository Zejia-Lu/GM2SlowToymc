#ifndef GSSIMULATOR_H
#define GSSIMULATOR_H

#include "TFile.h"
#include "TString.h"
#include "TH2D.h"
#include "MuonGenerator.hh"
#include "GSController.hh"
#include <memory>

class GSSimulator {

public:
    GSSimulator(int run_num = 0, const char* out_dir = ".");
    GSSimulator(GSController &controller);
    ~GSSimulator() = default;

    void Run();

    void Set_num_of_fill(long num_of_fill) {
        num_of_fill_ = num_of_fill;
    }

private:
    int verbose_ = 1;

    int run_num_ = 0;
    long num_of_fill_ = 100;
    TString out_dir_;
    bool is_uniform_energy_ = false;

    TFile* out_file_;
    std::shared_ptr<TH2D> hist2d_time_energy_;
    std::shared_ptr<TH1D> hist1d_time_lost_;
    std::shared_ptr<TH2D> hist2d_time_energy_inverse_ratio_;
    std::shared_ptr<TH1D> hist1d_time_lost_inverse_ratio_;
};

#endif