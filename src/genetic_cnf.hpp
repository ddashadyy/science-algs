#ifndef GENETIC_CHF_HPP
#define GENETIC_CHF_HPP

#include "cnf.hpp"
#include <utility>
    

class GeneticAlgorithm
{
public:

    GeneticAlgorithm(
        std::size_t iterations,
        std::size_t function_length,
        std::size_t population, 
        std::size_t hybridizations,
        std::size_t mutations,
        std::size_t amount_gens_mutations,
        selection_function sf
    );
    ~GeneticAlgorithm();

   std::pair<std::size_t, std::string> execute();

private:

    void hybridizate(std::vector<Candidate>& candidates);
    void mutate(std::vector<Candidate>& candidates);
    void do_selection(std::vector<Candidate>& candidates);

    CNF _cnf{};
    Candidates _candidates{}; 

    std::size_t _iterations;
    std::size_t _function_length;
    std::size_t _population;
    std::size_t _hybridizations;
    std::size_t _mutations;
    std::size_t _amount_gens_mutation;
    selection_function _sf;
};


#endif // GENETIC_CHF_HPP