#include "HInt.hpp"
using namespace std::complex_literals;
const double HBAR = 1.054571800e-34;

HInt::HInt(std::string fname):nstates(3) {
    double decay_rate, low_energy, high_energy;
    load_params(fname,
        {
            {"spontaneous_decay_rate", &decay_rate},
            {"enable_decay", &enable_decay},
            {"branching_ratio", &branching_ratio},
            {"low_energy_level", &low_energy},
            {"high_energy_level", &high_energy},
            {"rabi_frequency", &rabi_freq_per_decay},
            {"rabi_switch_coeff", &rabi_switch_coeff},
            {"rabi_switch_power", &rabi_switch_power},
            {"detuning_amplitude", &detun_amp_per_decay},
            {"detuning_frequency", &detun_freq_per_decay}
        }
    );
    transition_angfreq_per_decay = (high_energy - low_energy)/(HBAR*decay_rate);
}

unsigned HInt::subidx(unsigned i, unsigned j) {
    return nstates*i + j;
}

double HInt::rabi_softswitch(double gt) {
    double _;
    return rabi_freq_per_decay*std::exp(-rabi_switch_coeff*std::pow(
        std::abs(2*modf(detun_freq_per_decay*gt, &_) - 1), rabi_switch_power));
}

double HInt::detun_per_decay(double gt) {
    double _;
    return detun_amp_per_decay * (2*modf(detun_freq_per_decay*gt, &_) - 1);
}

double HInt::cumulative_phase(double gt) {
    double ncycles;
    double cycle_completion = modf(gt*detun_freq_per_decay, &ncycles);
    // Phase from full cycles + the phase from the current one
    return (ncycles * transition_angfreq_per_decay  // Full cycles
        + cycle_completion  // Current cycle
            * (transition_angfreq_per_decay
                + detun_amp_per_decay*(cycle_completion - 1))
        ) / detun_freq_per_decay;
}

std::vector<std::complex<double>> HInt::density_matrix(
    double gt, const std::vector<std::complex<double>>& coefficients) {
    std::complex<double> cexp = std::exp(1i*cumulative_phase(gt));
    return {
        coefficients[subidx(0,0)],
        coefficients[subidx(0,1)],
        coefficients[subidx(0,2)],
        coefficients[subidx(1,0)],
        coefficients[subidx(1,1)],
        coefficients[subidx(1,2)]*cexp,
        coefficients[subidx(2,0)],
        coefficients[subidx(2,1)]*std::conj(cexp),
        coefficients[subidx(2,2)]
    };
}

// Assumes row major format
std::vector<std::complex<double>> HInt::operator()(double gt,
    const std::vector<std::complex<double>>& rho_c) {
    // 1/(i*HBAR) * [H, rho_c] + L(rho_c) from the master equation
    return {
        (1 - branching_ratio)*rho_c[subidx(2,2)] * enable_decay,
        0.5i*(detun_per_decay(gt)*rho_c[subidx(0,1)]
            + rabi_softswitch(gt)*rho_c[subidx(0,2)]),
        -0.5*rho_c[subidx(0,2)] * enable_decay
            + 0.5i*(-detun_per_decay(gt)*rho_c[subidx(0,2)]
            + rabi_softswitch(gt)*rho_c[subidx(0,1)]),

        -0.5i*(detun_per_decay(gt)*rho_c[subidx(1,0)]
            + rabi_softswitch(gt)*rho_c[subidx(2,0)]),
        branching_ratio*rho_c[subidx(2,2)] * enable_decay
            + 0.5i*rabi_softswitch(gt)*(rho_c[subidx(1,2)]-rho_c[subidx(2,1)]),
        -0.5*rho_c[subidx(1,2)] * enable_decay
            + 1i*(0.5*rabi_softswitch(gt)*(rho_c[subidx(1,1)]-rho_c[subidx(2,2)])
            - detun_per_decay(gt)*rho_c[subidx(1,2)]),
        
        -0.5*rho_c[subidx(2,0)] * enable_decay
            + 0.5i*(detun_per_decay(gt)*rho_c[subidx(2,0)]
            - rabi_softswitch(gt)*rho_c[subidx(1,0)]),
        -0.5*rho_c[subidx(2,1)] * enable_decay
            - 1i*(0.5*rabi_softswitch(gt)*(rho_c[subidx(1,1)]-rho_c[subidx(2,2)])
            - detun_per_decay(gt)*rho_c[subidx(2,1)]),
        -rho_c[subidx(2,2)] * enable_decay
            - 0.5i*rabi_softswitch(gt)*(rho_c[subidx(1,2)]-rho_c[subidx(2,1)])
    };
}
