#include "MuonGenerator.hh"
#include "TFile.h"
#include "TROOT.h"
#include <memory>
#include <algorithm>

MuonGenerator::MuonGenerator(uint seed)
{
    rand_gen_ = TRandom3(seed);

    energy_dist_ = std::make_shared<TF1>("energy_dist", "(x-1) * (4*x^2-5*x-5) * (1 - [0] * (-8*x^2+x+1) / (4*x^2-5*x-5) * cos([1]) )", 0, 1);
    energy_dist_->SetParNames("polarization", "phase");
    energy_dist_->SetParameters(polarization_, 0);

    wiggle_ = std::make_shared<TF1>("wiggle", "exp(-x/[0]) * (1 + [1] * cos([2]*x + [3]))", t_start_, 700);
    wiggle_->SetParNames("lifetime", "asymmetry", "omega_a", "phi0");
    wiggle_->SetParameters(muon_lifetime_, asymmetry_, omega_a_, phi0_);
    wiggle_->SetNpx(100000);
}

void MuonGenerator::Init()
{
    mc_muons_.clear();
}

void MuonGenerator::Generate_decay()
{
    if (mc_muons_.size() != 0) {
        std::cerr << "[error] Init() before Generate() to remove generated muons!" << std::endl;
    }

    // Generate decay muons.
    for (int i = 0; i < muons_per_fill_; i++) {
        double decay_time = rand_gen_.Exp(muon_lifetime_) + t_start_;
        double decay_energy = Get_energy_from_time(decay_time);
        double decay_x = rand_gen_.Gaus(0, 10);
        double decay_y = rand_gen_.Gaus(0, 10);

        auto new_muon = std::make_shared<MCMuon>(decay_energy, decay_time, decay_x, decay_y);
        mc_muons_.push_back(new_muon);
    }

    // Sort generated muons according to decay time.
    auto compare_time = [](const MCMuonPtr p1, const MCMuonPtr p2) {
        return p1->decay_time < p2->decay_time;
    };

    std::sort(mc_muons_.begin(), mc_muons_.end(), compare_time);
}


void MuonGenerator::Generate_decay_unitEnergy()
{
    if (mc_muons_.size() != 0) {
        std::cerr << "[error] Init() before Generate() to remove generated muons!" << std::endl;
    }

    // Generate decay muons.
    for (int i = 0; i < muons_per_fill_; i++) {
        double decay_time = wiggle_->GetRandom();
        double decay_energy = rand_gen_.Uniform(0, E_max_);
        double decay_x = rand_gen_.Gaus(0, 10);
        double decay_y = rand_gen_.Gaus(0, 10);

        auto new_muon = std::make_shared<MCMuon>(decay_energy, decay_time, decay_x, decay_y);
        mc_muons_.push_back(new_muon);
    }

    // Sort generated muons according to decay time.
    auto compare_time = [](const MCMuonPtr p1, const MCMuonPtr p2) {
        return p1->decay_time < p2->decay_time;
    };

    std::sort(mc_muons_.begin(), mc_muons_.end(), compare_time);
}

// Generate lost muon
void MuonGenerator::Generate_lost()
{
    if (hist_lost_muon_norm_ == nullptr) {
        std::cerr << "[error] Not lost muon spectrum given! Not lost muon will be generated!" << std::endl;
        return;
    }

    double average_lost_num = lost_rate_ * muons_per_fill_;
    int lost_num = rand_gen_.Poisson(average_lost_num);

    std::vector<double> lost_times = {};
    for (int i = 0; i < lost_num; i++) {
        lost_times.push_back(hist_lost_muon_norm_->GetRandom());
    }

    std::sort(lost_times.begin(), lost_times.end());

    std::vector<int> decay_nos = {};
    std::vector<double> decay_times = {};
    for (int i = 0; i < mc_muons_.size(); i++) {
        decay_nos.push_back(i);
        decay_times.push_back(mc_muons_.at(i)->decay_time);
    }

    std::vector<int> lost_nos = {};
    for (int i = 0; i < lost_num; i++) {
        auto it = std::lower_bound(decay_times.begin(), decay_times.end(), lost_times.at(i));
        int no = std::distance(decay_times.begin(), it);
        int num_residual = std::distance(it, decay_times.end());

        if (num_residual == 0) break;

        int lost_no = no + rand_gen_.Integer(num_residual);

        decay_times.erase(decay_times.begin() + lost_no);
        lost_nos.push_back(*(decay_nos.begin() + lost_no));
        decay_nos.erase(decay_nos.begin() + lost_no);
    }

    for (int i = 0; i < lost_nos.size(); i++) {
        int lost_no = lost_nos.at(i);
        mc_muons_.at(lost_no)->lost = true;
        mc_muons_.at(lost_no)->lost_time = lost_times.at(i);
    }
}

// Generate decay muon energy from decay time (in us)
double MuonGenerator::Get_energy_from_time(double time)
{
    double phi = omega_a_ * time + phi0_;
    energy_dist_->SetParameter("phase", phi);

    return energy_dist_->GetRandom() * E_max_;
}

void MuonGenerator::Set_lost_muon(TString file_name, TString hist_name)
{
    auto in_file = TFile::Open(file_name);
    auto lm_hist = in_file->Get<TH1D>(hist_name);

    hist_lost_muon_norm_ = std::shared_ptr<TH1D>(static_cast<TH1D*>(lm_hist->Clone()));

    for (int i = 0; i < hist_lost_muon_norm_->GetNbinsX(); i++) {
        if (hist_lost_muon_norm_->GetBinCenter(i) < t_start_) {
            hist_lost_muon_norm_->SetBinContent(i, 0);
        }
    }

    // in_file->Close();
}