#include "genetic_cnf.hpp"

#include <random>
#include <algorithm>
#include <iostream>
#include <limits>

constexpr double EPSILON = std::numeric_limits<double>::epsilon() * 10;

GeneticAlgorithm::GeneticAlgorithm(
    std::size_t iterations,
    std::size_t function_length,
    std::size_t population, 
    std::size_t hybridizations, 
    std::size_t mutations, 
    std::size_t amount_gens_mutations, 
    selection_function sf
) : _iterations(iterations), _function_length(function_length) ,_population(population), _hybridizations(hybridizations), 
    _mutations(mutations), _amount_gens_mutation(amount_gens_mutations), _sf(sf)
{
    this->_cnf = CNF(function_length);
    this->_candidates = Candidates(population, function_length);
}

GeneticAlgorithm::~GeneticAlgorithm() = default;

std::pair<std::size_t, std::string> GeneticAlgorithm::execute() 
{
    auto& candidates = this->_candidates.get_candidates();
    std::size_t iteration_counter = 1;

    for (std::size_t i = 0; i < this->_iterations; i++)
    {    
        for (auto& candidate : candidates)
        {
            candidate.evaluate_quality_function(this->_cnf);
            if (std::fabs(candidate.get_quality() - 1.0) < EPSILON) 
                return std::make_pair(iteration_counter, candidate.get_function());
            
        }
        hybridizate(candidates);
        mutate(candidates);
        do_selection(candidates);

        ++iteration_counter;
    }

    return std::make_pair(iteration_counter, std::string{"there is no solution"});
}

void GeneticAlgorithm::hybridizate(std::vector<Candidate>& candidates)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> distrib(0, this->_candidates.get_candidates()[0].get_function().size() - 1);

    for (std::size_t i = 0; i < this->_hybridizations; i += 2)
    {
        std::size_t first_parent_index = 0;
        std::size_t second_parent_index = 0;

        switch (this->_sf)
        {
        case selection_function::RANDOM:
            first_parent_index = distrib(gen);
            break;

        case selection_function::LINEAR:
            first_parent_index = i % (this->_population - 1);
            second_parent_index = i % (this->_population - 1);
            break;

        case selection_function::EXPONENTIAL:
            first_parent_index = (i * i) % (this->_population - 1);
            second_parent_index = (i * i) % (this->_population - 1);
            break;
        
        default:
            break;
        }

        std::size_t point = distrib(gen);

        std::string child = 
            candidates[first_parent_index].get_function().substr(0, point) + 
            candidates[second_parent_index].get_function().substr(point);

        candidates.emplace_back(child);
    }
}

void GeneticAlgorithm::mutate(std::vector<Candidate>& candidates)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> distrib(0, this->_population - 1);

    for (std::size_t i = 0; i < this->_mutations; i++)
    {
        std::size_t candidate_index_to_mutate = 0;

        switch (this->_sf)
        {
        case selection_function::RANDOM:
            candidate_index_to_mutate = distrib(gen);
            break;

        case selection_function::LINEAR:
            candidate_index_to_mutate = i % (this->_population - 1);
            break;

        case selection_function::EXPONENTIAL:
            candidate_index_to_mutate = (i * i) % (this->_population - 1);
            break;
        
        default:
            break;
        }

        for (std::size_t j = 0; j < this->_amount_gens_mutation; j++)
        {
            std::uniform_int_distribution<std::size_t> distrib(0, candidates[i].get_function().size() - 1);
            std::size_t mutation_point = distrib(gen);

            candidates[candidate_index_to_mutate].get_function()[mutation_point] = 
                (candidates[candidate_index_to_mutate].get_function()[mutation_point] == '0') ? '1' : '0';
        }
    }
   
}

void GeneticAlgorithm::do_selection(std::vector<Candidate> &candidates)
{
    std::sort(candidates.begin(), candidates.end(), [](const Candidate& lhs, const Candidate& rhs){
        return lhs.get_quality() < rhs.get_quality();
    });

    candidates.resize(this->_population);
}
