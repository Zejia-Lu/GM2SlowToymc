#ifndef MUONGENERATOR_H
#define MUONGENERATOR_H

#include "MCMuon.hh"
#include "TRandom3.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1D.h"

#include <vector>
#include <memory>

typedef std::shared_ptr<MCMuon> MCMuonPtr;

class MuonGenerator {

public:
    MuonGenerator(uint seed);
    ~MuonGenerator() = default;

    // Clear MCMuons
    void Init();

    // Generate MCMuons with decay time, energy...
    void Generate_decay();

    // Generate lost muon effect
    void Generate_lost();

    // Generate MCMuons with uniform energy distribution, for cross check
    void Generate_decay_unitEnergy();

    std::vector<MCMuonPtr> Get_mc_muons() {
        return mc_muons_;
    }
    double Get_omega_a() {return omega_a_;}

    void Set_lost_muon(TString file_name, TString hist_name);
    void Set_lost_rate(double lost_rate) { lost_rate_ = lost_rate; }
    void Set_muons_per_fill(int number) {muons_per_fill_ = number; }

private:
    TRandom3 rand_gen_;

    int verbose_ = 1;

    int muons_per_fill_ = 500;
    double t_start_ = 30; // in us

    double polarization_ = 0.95;
    double E_max_ = 3100; // in MeV
    double muon_lifetime_ = 64.4; // in us
    double asymmetry_ = 0.36;
    double omega_a_ = 2 * TMath::Pi() * 0.2291; // in MHz * rad
    double phi0_ = 2.075;

    // Normalized energy distribution (depend on polarization and phase)
    std::shared_ptr<TF1> energy_dist_ = nullptr;
    double Get_energy_from_time(double time);

    // For lost muon modelling.
    std::shared_ptr<TH1D> hist_lost_muon_norm_ = nullptr;
    double lost_rate_ = 1e-2;

    // Wiggle function for uniform energy distribution.
    std::shared_ptr<TF1> wiggle_ = nullptr;

    std::vector<MCMuonPtr> mc_muons_;
};

#endif