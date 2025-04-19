#ifndef GENETIC_CHF_HPP
#define GENETIC_CHF_HPP

#include "cnf.hpp"

    // параметры генетического алгоритма
    // 1. популяция
    // 2. количество скрещиваний
    // 3. количество мутаций
    // 4. функция выбора на скрещивание и мутацию
    //    случайная, линейная, экспоненциальная
    // 5. количество генов при мутации 


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

    std::string execute();

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


// // скрещивание
// void hybridization(size_t amount_hybridization, selection_function sf, size_t population);

// // мутиация
// void mutate(size_t amount_mutations, selection_function sf, size_t amount_gens_mutatuions);

// // компаратор для отбора кандидата
// int compare_candidates(const void* lhs, const void* rhs);

// // отбор
// void selection(size_t population_size, size_t amount_hybridization);

// // генетический алгоритм
// char* genetic_algorithm(genetic_alg_params* params);


#endif // GENETIC_CHF_HPP