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

void GSSimulator::Run()
{
    auto start = std::chrono::high_resolution_clock::now();
    auto currentTimeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(start.time_since_epoch()).count();


    // Output control
    out_file_ = TFile::Open(Form("%s/toymc_run%d.root", out_dir_.Data(), run_num_), "recreate");
    hist2d_time_energy_ = 
        std::make_shared<TH2D>("hist2d_t_e", "hist2d_t_e;time [#mus];energy [MeV]", 4500, 0, 671.4, 400, 0, 4000);
    hist2d_time_energy_inverse_ratio_ = 
        std::make_shared<TH2D>("hist2d_t_e_inv_ratio", "hist2d_t_e_inv_ratio;time [#mus];energy [MeV]", 4500, 0, 671.4, 400, 0, 4000);
    hist1d_time_lost_ = 
        std::make_shared<TH1D>("hist1d_time_lost", "hist_lm;time [#mus]", 4500, 0, 671.4);
    hist1d_time_lost_inverse_ratio_ = 
        std::make_shared<TH1D>("hist1d_time_lost_inv_ratio", "hist_lm_inv_ratio;time [#mus]", 4500, 0, 671.4);


    // Initialize positrons generator.
    uint seed = run_num_ * 1000000 + currentTimeInSeconds;
    PositronGenerator pos_generator(seed);
    pos_generator.Set_lost_muon("/lustre/collider/luzejia/gm2/res_slow_toymc/data/lostmuon_run4all.root", "lostMuon/hist_lm");
    pos_generator.Set_lost_rate(1e-2);


    // Randomization for inverse ratio method.
    uint seed_ir = run_num_ * 1010000 + currentTimeInSeconds;
    TRandom3 rand_gen_ir(seed_ir);
    double T_a = 2 * TMath::Pi() / pos_generator.Get_omega_a();


    // Start generation.
    for (int i = 0; i < num_of_fill_; i++) {

        // if ((i + 1) % (num_of_fill_ / 10) == 0) {
        std::cout << "Generating the " << i + 1 << " fill..." << std::endl;
        // }

        pos_generator.Init();
        // pos_generator.Generate_decay();
        pos_generator.Generate_decay_unitEnergy();
        pos_generator.Generate_lost();

        for (auto mc_positron : pos_generator.Get_mc_positrons()) {
            double random_omega_a = rand_gen_ir.Uniform(-T_a/2, T_a/2);
            if (!mc_positron->lost) {
                hist2d_time_energy_->Fill(mc_positron->decay_time, mc_positron->decay_energy);
                hist2d_time_energy_inverse_ratio_->Fill(mc_positron->decay_time + random_omega_a, mc_positron->decay_energy);
            } else {
                hist1d_time_lost_->Fill(mc_positron->lost_time);
                hist1d_time_lost_inverse_ratio_->Fill(mc_positron->lost_time + random_omega_a);
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
    std::cout << "Time taken by Generate_decay(): " << duration.count() << " ms" << std::endl;
}