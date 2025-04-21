#ifndef GENETIC_CHF_HPP
#define GENETIC_CHF_HPP

#include "cnf.hpp"

#include <utility>
#include <tuple>

class GeneticAlgorithm
{
public:
    GeneticAlgorithm(
        const CNF& cnf,
        const Candidates& candidates,
        selection_function sf);
    ~GeneticAlgorithm();

    std::tuple<std::size_t, std::vector<double>, std::string> execute(
        std::size_t iterations,
        std::size_t population,
        std::size_t hybridizations,
        std::size_t mutations,
        std::size_t amount_gens_mutations
    );

private:
    void hybridizate(
        std::vector<Candidate>& candidates,
        std::size_t hybridizations,
        std::size_t population
    );
    void mutate(
        std::vector<Candidate>& candidates,
        std::size_t population,
        std::size_t mutations,
        std::size_t amount_gens_mutations
    );
    void do_selection(
        std::vector<Candidate>& candidates,
        std::size_t population
    );

    CNF _cnf;
    Candidates _candidates;
    selection_function _sf;
};

#endif // GENETIC_CHF_HPP