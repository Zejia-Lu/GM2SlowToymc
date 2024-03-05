#include "GSSimulator.hh"
#include "TH2D.h"
#include "TRandom3.h"
#include "TMath.h"
#include <chrono>
#include <memory>

GSSimulator::GSSimulator(int run_num, const char* out_dir)
    : run_num_(run_num),
      out_dir_(out_dir)
{
}

GSSimulator::GSSimulator(GSController &controller)
{
    run_num_           = controller.run_num;
    num_of_fill_       = controller.num_of_fill;
    out_dir_           = controller.out_dir;
    is_uniform_energy_ = controller.is_uniform_energy;

    lost_rate_           = controller.lost_rate;
    lost_muon_path_      = controller.lost_muon_path;
    lost_muon_root_path_ = controller.lost_muon_root_path;
}

void GSSimulator::Run()
{
    auto start = std::chrono::high_resolution_clock::now();
    auto currentTimeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(start.time_since_epoch()).count();


    // Output control
    out_file_ = TFile::Open(Form("%s/toymc_run%d.root", out_dir_.Data(), run_num_), "recreate");
    if (out_file_ == nullptr) {
        std::cerr << "[Error] Cannot open file " << Form("%s/toymc_run%d.root", out_dir_.Data(), run_num_) 
                  << ", please try to modify 'out_dir' in config file."
                  << std::endl;
        return;
    }

    hist2d_time_energy_ = 
        std::make_shared<TH2D>("hist2d_t_e", "hist2d_t_e;time [#mus];energy [MeV]", 4500, 0, 671.4, 400, 0, 4000);
    hist2d_time_energy_inverse_ratio_ = 
        std::make_shared<TH2D>("hist2d_t_e_inv_ratio", "hist2d_t_e_inv_ratio;time [#mus];energy [MeV]", 4500, 0, 671.4, 400, 0, 4000);
    hist1d_time_lost_ = 
        std::make_shared<TH1D>("hist1d_time_lost", "hist_lm;time [#mus]", 4500, 0, 671.4);
    hist1d_time_lost_inverse_ratio_ = 
        std::make_shared<TH1D>("hist1d_time_lost_inv_ratio", "hist_lm_inv_ratio;time [#mus]", 4500, 0, 671.4);


    // Initialize muons generator.
    uint seed = run_num_ * 1000000 + currentTimeInSeconds;
    MuonGenerator muon_generator(seed);
    muon_generator.Set_lost_muon(lost_muon_path_, lost_muon_root_path_);
    muon_generator.Set_lost_rate(lost_rate_);


    // Randomization for inverse ratio method.
    uint seed_ir = run_num_ * 1010000 + currentTimeInSeconds;
    TRandom3 rand_gen_ir(seed_ir);
    double T_a = 2 * TMath::Pi() / muon_generator.Get_omega_a();


    // Start generation.
    for (int i = 0; i < num_of_fill_; i++) {

        // if ((i + 1) % (num_of_fill_ / 10) == 0) {
        std::cout << "\rGenerating the (" << i + 1 << "/" << num_of_fill_ << ") fill...";
        // }

        muon_generator.Init();
        
        if (!is_uniform_energy_) {
            muon_generator.Generate_decay();
        } else {
            muon_generator.Generate_decay_unitEnergy();
        }
        
        muon_generator.Generate_lost();

        for (auto mc_muon : muon_generator.Get_mc_muons()) {
            double random_omega_a = rand_gen_ir.Uniform(-T_a/2, T_a/2);
            if (!mc_muon->lost) {
                hist2d_time_energy_->Fill(mc_muon->decay_time, mc_muon->decay_energy);
                hist2d_time_energy_inverse_ratio_->Fill(mc_muon->decay_time + random_omega_a, mc_muon->decay_energy);
            } else {
                hist1d_time_lost_->Fill(mc_muon->lost_time);
                hist1d_time_lost_inverse_ratio_->Fill(mc_muon->lost_time + random_omega_a);
            }
        }
    }


    // Write histogram.
    out_file_->cd();
    hist2d_time_energy_->Write();
    hist1d_time_lost_->Write();
    hist2d_time_energy_inverse_ratio_->Write();
    hist1d_time_lost_inverse_ratio_->Write();
    out_file_->Close();


    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\nTime taken by Generate_decay(): " << duration.count() << " ms" << std::endl;
}