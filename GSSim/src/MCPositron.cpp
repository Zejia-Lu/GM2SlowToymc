#include "MCPositron.hh"
#include "TString.h"

MCPositron::MCPositron(double decay_energy, double decay_time, double decay_x, double decay_y)
    : decay_energy(decay_energy),
      decay_time(decay_time),
      decay_x(decay_x),
      decay_y(decay_y)
{
}

std::ostream& operator<<(std::ostream& os, const MCPositron& obj) {
    auto output = Form("(e=%.2f, t=%.2f, x=%.2f, y=%.2f)", obj.decay_energy, obj.decay_time, obj.decay_x, obj.decay_y);
    os << output;
    return os;
}