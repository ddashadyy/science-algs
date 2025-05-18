#ifndef SA_CNF_HPP
#define SA_CNF_HPP

#include "cnf.hpp"


class SaAlgorithm final
{
public:
    SaAlgorithm(
        const CNF& cnf,
        const Candidate& candidate
    );
    ~SaAlgorithm();

    Candidate execute(
        double initial_temp,
        double min_temp,
        double cooling_rate,
        cooling_type ct,
        std::size_t iterations
    );

    std::vector<double> get_quality_history() const;

private:

    double eval_energy(Candidate& candidate) const;
    Candidate get_neighbor() const;
    bool accept_solution(double current_enegry, double new_energy, double temp) const;
    double cool(
        double initial_temp, 
        double min_temp, 
        double cooling_rate, 
        cooling_type ct,
        size_t iteration, 
        size_t max_iterations
    ) const;

    CNF cnf_;
    Candidate candidate_;

    std::vector<double> quality_history_{};

};



#endif // SA_CNF_HPP
