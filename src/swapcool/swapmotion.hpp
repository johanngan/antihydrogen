#ifndef SWAPMOTION_HPP_
#define SWAPMOTION_HPP_

#include <cmath>
#include <iomanip>
#include <string>
#include <fstream>
#include <complex>
#include <vector>
#include <chrono>
#include <gsl/gsl_integration.h>
#include "HMotion.hpp"
#include "DensMatHandler.hpp"
#include "lasercool/readcfg.hpp"
#include "lasercool/iotag.hpp"
#include "lasercool/timestepping.hpp"
#include "lasercool/fundconst.hpp"

// Generate a thermal state
std::vector<std::complex<double>> thermal_state(double, const HMotion&);
// Numerically compute the integral for the weight for an axial momentum state
// under a given thermal standard deviation
double k_axial_distr(double, double, gsl_integration_workspace*, double, double);
// Generate the antihydrogen 2s "thermal-derived" state
std::vector<std::complex<double>> antihydrogen_2s_state(double, const HMotion&);
// Print out information about the system
void print_system_info(const std::vector<std::complex<double>>&,
    const HMotion&, double, double, bool, bool, double, double);
// Calculate the RMS k value of a state
double calc_krms(const std::vector<std::complex<double>>&,
    const DensMatHandler&);
// Calculate the RMS k value within the population that hasn't leaked yet
double calc_krms_unleaked(const std::vector<std::complex<double>>&,
    const DensMatHandler&);
// Quality metric evaluation string
std::string evaluate_quality_metric(
    double,
    double low_thresh=1,
    double very_low_thresh=0.5,
    std::string okay_str="",
    std::string low_str="*",
    std::string very_low_str="**");
// Write state info to a file given the density matrix at a fixed time
void write_state_info(std::ofstream&, double,
    const std::vector<std::complex<double>>&, const DensMatHandler&);
// Write the k-distribution at a fixed time to a file in tall format
void write_kdist(std::ofstream&, double,
    const std::vector<std::complex<double>>&, const DensMatHandler&);

#endif