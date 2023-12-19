#include <iostream>

class MCPositron {

public:
    MCPositron(double decay_energy, double decay_time, double decay_x, double decay_y);
    ~MCPositron() = default;

    double decay_energy;
    double decay_time;
    double decay_x;
    double decay_y;

    bool lost = false;
    double lost_time = 0;

    double recon_energy;
    double recon_time;

    friend std::ostream& operator<<(std::ostream& os, const MCPositron& obj);

private:
};