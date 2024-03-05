This is a toy simulation program for simulating wiggle, lost muon, and beam dynamic effect (to be added) in muon g-2 experiment.

## Quick Start
After clone the repo, you can build the program with cmake
```sh
mkdir build && cd build
cmake ../
make
```
An executable file named "GSSim" will show up in the build directory. The program can be runned by
```sh
./GSSim -c <config_file>
```
where example of <config_file> can be found in GM2SlowToymc/script/config.txt.
