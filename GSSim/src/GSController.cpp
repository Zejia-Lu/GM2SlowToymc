#include "GSController.hh"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

void GSController::Get_config(const char* filename)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(filename, pt);

    run_num           = pt.get<int>("control.run_num", 0);
    num_of_fill       = pt.get<int>("control.num_of_fill", 100);
    out_dir           = pt.get<TString>("control.out_dir", "./");
    is_uniform_energy = pt.get<bool>("control.is_uniform_energy", false);
}
