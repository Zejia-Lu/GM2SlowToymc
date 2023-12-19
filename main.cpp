#include "GSSimulator.hh"
#include "TString.h"

int main(int argc, char **argv) {
    // Default parameters
    int run_num = 0;
    long num_of_fill = 100;
    TString out_dir(".");

    // Read input
    for (int i = 0; i < argc; ++i) {

        if (std::string(argv[i]) == "-r") {
            if (i + 1 < argc)
                run_num = std::atoi(argv[i + 1]);

        } else if (std::string(argv[i]) == "-n") {
            if (i + 1 < argc)
                num_of_fill = std::atol(argv[i + 1]);

        } else if (std::string(argv[i]) == "-o") {
            if (i + 1 < argc)
                out_dir = argv[i + 1];
        }
    }

    // Start simulator
    GSSimulator simulator(run_num, out_dir);
    simulator.Set_num_of_fill(num_of_fill);
    simulator.Run();

    return 0;
}