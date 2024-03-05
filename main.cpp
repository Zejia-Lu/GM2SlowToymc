#include "GSSimulator.hh"
#include "GSController.hh"
#include "TString.h"

int main(int argc, char **argv) {
    // Get config from file
    TString config_dir;
    GSController controller;

    for (int i = 0; i < argc; ++i) {
        if (std::string(argv[i]) == "-c" || std::string(argv[i]) == "-config") {
            if (i + 1 < argc)
                config_dir = argv[i + 1];
                controller.Get_config(config_dir);
        }
    }

    // Read input from shell
    for (int i = 0; i < argc; ++i) {

        if (std::string(argv[i]) == "-r" || std::string(argv[i]) == "-run") {
            if (i + 1 < argc)
                controller.run_num = std::atoi(argv[i + 1]);

        } else if (std::string(argv[i]) == "-n" || std::string(argv[i]) == "-nfill") {
            if (i + 1 < argc)
                controller.num_of_fill = std::atol(argv[i + 1]);

        } else if (std::string(argv[i]) == "-o" || std::string(argv[i]) == "-out") {
            if (i + 1 < argc)
                controller.out_dir = argv[i + 1];
        }
    }

    // Start simulator
    GSSimulator simulator(controller);
    simulator.Run();

    return 0;
}