#include "PositronGenerator.hh"
#include "TFile.h"
#include "TROOT.h"
#include <memory>
#include <algorithm>

PositronGenerator::PositronGenerator(uint seed)
{
    rand_gen_ = TRandom3(seed);

    energy_dist_ = std::make_shared<TF1>("energy_dist", "(x-1) * (4*x^2-5*x-5) * (1 - [0] * (-8*x^2+x+1) / (4*x^2-5*x-5) * cos([1]) )", 0, 1);
    energy_dist_->SetParNames("polarization", "phase");
    energy_dist_->SetParameters(polarization_, 0);
}

void PositronGenerator::Init()
{
    mc_positrons_.clear();
}

void PositronGenerator::Generate_decay()
{
    if (mc_positrons_.size() != 0) {
        std::cerr << "[error] Init() before Generate() to remove generated positrons!" << std::endl;
    }


    // Generate decay positrons.
    for (int i = 0; i < positrons_per_fill_; i++) {
        double decay_time = rand_gen_.Exp(muon_lifetime_) + t_start_;
        double decay_energy = Get_energy_from_time(decay_time);
        double decay_x = rand_gen_.Gaus(0, 10);
        double decay_y = rand_gen_.Gaus(0, 10);

        auto new_positron = std::make_shared<MCPositron>(decay_energy, decay_time, decay_x, decay_y);
        mc_positrons_.push_back(new_positron);
    }


    // Sort generated positrons according to decay time.
    auto compare_time = [](const MCPositronPtr p1, const MCPositronPtr p2) {
        return p1->decay_time < p2->decay_time;
    };

    std::sort(mc_positrons_.begin(), mc_positrons_.end(), compare_time);
}


// Generate lost muon
void PositronGenerator::Generate_lost()
{
    if (hist_lost_muon_norm_ == nullptr) {
        std::cerr << "[error] Not lost muon spectrum given! Not lost muon will be generated!" << std::endl;
        return;
    }

    double average_lost_num = lost_rate_ * positrons_per_fill_;
    int lost_num = rand_gen_.Poisson(average_lost_num);

    std::vector<double> lost_times = {};
    for (int i = 0; i < lost_num; i++) {
        lost_times.push_back(hist_lost_muon_norm_->GetRandom());
    }

    std::sort(lost_times.begin(), lost_times.end());

    std::vector<double> decay_times = {};
    for (auto mc_positron : mc_positrons_) {
        decay_times.push_back(mc_positron->decay_time);
    }

    std::vector<int> lost_nos = {};
    for (int i = 0; i < lost_num; i++) {
        auto it = std::lower_bound(decay_times.begin(), decay_times.end(), lost_times.at(i));
        int no = std::distance(decay_times.begin(), it);
        int num_residual = std::distance(it, decay_times.end());

        if (num_residual == 0) break;

        int lost_no = no + rand_gen_.Integer(num_residual);
        while (std::find(lost_nos.begin(), lost_nos.end(), lost_no) != lost_nos.end()) {
            lost_no = no + rand_gen_.Integer(num_residual);
        }
        
        lost_nos.push_back(lost_no);
    }

    for (int i = 0; i < lost_nos.size(); i++) {
        int lost_no = lost_nos.at(i);
        mc_positrons_.at(lost_no)->lost = true;
        mc_positrons_.at(lost_no)->lost_time = lost_times.at(i);
    }
}

// Generate decay positron energy from decay time (in us)
double PositronGenerator::Get_energy_from_time(double time)
{
    double phi = omega_a_ * time + phi0_;
    energy_dist_->SetParameter("phase", phi);

    return energy_dist_->GetRandom() * E_max_;
}

void PositronGenerator::Set_lost_muon(TString file_name, TString hist_name)
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