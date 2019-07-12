#ifndef HMOTION_HPP_
#define HMOTION_HPP_

#ifdef _OPENMP
#include <omp.h>
#endif

#include "HSwap.hpp"
#include "DensMatHandler.hpp"

// Hamiltonian for sawtooth laser frequency oscillating about
// some transition frequency, under the rotating wave approximation,
// including interaction with the laser and also motional states
struct HMotion : public HSwap {
    const double SPEED_OF_LIGHT, K_BOLTZMANN;
    double recoil_freq_per_decay;
    DensMatHandler handler;
    // Probabilities from subdividing dipole radiation into sectors
    // Only tracks the forward region, from dk = 0 to dk = 1, in ascending order.
    // The value for dk = 0 is half the actual value, which will be double-counted
    // when accounting for the negative dk values.
    std::vector<double> diprad_probs;

    HMotion(std::string);

    // The action of the Hamiltonian on the density matrix, returns a single
    // component of H*rho
    // Optionally provide a precomputed stored index for speed
    // -1 means no index is provided
    std::complex<double> haction(const std::vector<std::complex<double>>&,
        unsigned, int, unsigned, int, int idx=-1) const;
    
    // The spontaneous decay part of the derivative (Lindblad superoperator)
    std::complex<double> decayterm(const std::vector<std::complex<double>>&,
        unsigned, int, unsigned, int, unsigned) const;

    // Transforms the coefficients solved for in the rotating wave
    // approximation back to the actual density matrix values;
    // i.e. put the oscillation back in.
    std::vector<std::complex<double>> density_matrix(
        double, const std::vector<std::complex<double>>&) const override;

    // Derivative operator to be passed to the timestepper
    std::vector<std::complex<double>> operator()(double,
        const std::vector<std::complex<double>>&) override;

    // Modify the density matrix in preparation for a new cycle
    void initialize_cycle(std::vector<std::complex<double>>&) const;
};

#endif