#include "MCPositron.hh"
#include "TRandom3.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1D.h"

#include <vector>
#include <memory>

typedef std::shared_ptr<MCPositron> MCPositronPtr;

class PositronGenerator {

public:
    PositronGenerator(uint seed);
    ~PositronGenerator() = default;

    void Init();

    void Generate_decay();
    void Generate_lost();

    std::vector<MCPositronPtr> Get_mc_positrons() {
        return mc_positrons_;
    }

    void Set_lost_muon(TString file_name, TString hist_name);

private:
    TRandom3 rand_gen_;

    int verbose_ = 1;

    int positrons_per_fill_ = 500;
    double t_start_ = 30; // in us

    double polarization_ = 0.95;
    double E_max_ = 3100; // in MeV
    double muon_lifetime_ = 64; // in us
    double phi0_ = 2.075;
    double omega_a_ = 2 * TMath::Pi() * 0.2291; // in MHz * rad

    // Normalized energy distribution (depend on polarization and phase)
    std::shared_ptr<TF1> energy_dist_;
    double Get_energy_from_time(double time);

    // For lost muon modelling.
    std::shared_ptr<TH1D> hist_lost_muon_norm_ = nullptr;
    double lost_rate_ = 1e-1;

    std::vector<MCPositronPtr> mc_positrons_;
};