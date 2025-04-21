#include "genetic_cnf.hpp"

#include <random>
#include <algorithm>
#include <iostream>
#include <limits>

constexpr double EPSILON = std::numeric_limits<double>::epsilon() * 10;

GeneticAlgorithm::GeneticAlgorithm(
    const CNF &cnf,
    const Candidates &candidates,
    selection_function sf) : _cnf(cnf), _candidates(candidates), _sf(sf) {}

GeneticAlgorithm::~GeneticAlgorithm() { std::cout << "GeneticAlgorithm destroyed\n"; }

std::pair<std::size_t, std::string> GeneticAlgorithm::execute(
    std::size_t iterations,
    std::size_t population,
    std::size_t hybridizations,
    std::size_t mutations,
    std::size_t amount_gens_mutations
)
{
    auto& candidates = this->_candidates.get_candidates();
    std::size_t iteration_counter = 1;

    for (std::size_t i = 0; i < iterations; i++)
    {
        for (auto& candidate : candidates)
        {
            candidate.evaluate_quality_function(this->_cnf);
            if (std::fabs(candidate.get_quality() - 1.0) < EPSILON)
                return std::make_pair(iteration_counter, candidate.get_function());
        }

        hybridizate(candidates, hybridizations, population);
        mutate(candidates, population, mutations, amount_gens_mutations);
        do_selection(candidates, population);

        ++iteration_counter;
    }

    return std::make_pair(iteration_counter, std::string{"there is no solution"});
}

void GeneticAlgorithm::hybridizate(
    std::vector<Candidate>& candidates,
    std::size_t hybridizations,
    std::size_t population
)
{
    if (candidates.empty() || candidates[0].get_function().empty() || candidates.size() < 2) 
        return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::size_t func_size = candidates[0].get_function().size();
    std::uniform_int_distribution<std::size_t> distrib(0, candidates.size() - 1);

    for (std::size_t i = 0; i < hybridizations && candidates.size() < population; i++)
    {
        std::size_t first_parent_index = 0;
        std::size_t second_parent_index = 0;

        do {
            switch (this->_sf)
            {
            case selection_function::RANDOM:
                first_parent_index = distrib(gen);
                do {
                    second_parent_index = distrib(gen);
                } while (second_parent_index == first_parent_index);
                break;

            case selection_function::LINEAR:
                first_parent_index = i % candidates.size();
                second_parent_index = (i + 1) % candidates.size();
                if (second_parent_index == first_parent_index) {
                    second_parent_index = (first_parent_index + 1) % candidates.size();
                }
                break;

            case selection_function::EXPONENTIAL:
                first_parent_index = (i * i) % candidates.size();
                second_parent_index = ((i + 1) * (i + 1)) % candidates.size();
                if (second_parent_index == first_parent_index) {
                    second_parent_index = (first_parent_index + 1) % candidates.size();
                }
                break;

            default:
                first_parent_index = 0;
                second_parent_index = 1 % candidates.size();
                break;
            }
        } while (first_parent_index == second_parent_index);

        std::uniform_int_distribution<std::size_t> distrib_point(1, func_size - 1);
        std::size_t point = distrib_point(gen);

        std::cout << point << '\n';
        std::cout << first_parent_index << ' ' << second_parent_index << '\n';

        std::string child =
            candidates[first_parent_index].get_function().substr(0, point) +
            candidates[second_parent_index].get_function().substr(point);

        candidates.emplace_back(child);
    }
}

void GeneticAlgorithm::mutate(
    std::vector<Candidate>& candidates,
    std::size_t population,
    std::size_t mutations,
    std::size_t amount_gens_mutations
)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> distrib(0, population - 1);

    for (std::size_t i = 0; i < mutations; i++)
    {
        std::size_t candidate_index_to_mutate = 0;

        switch (this->_sf)
        {
        case selection_function::RANDOM:
            candidate_index_to_mutate = distrib(gen);
            break;

        case selection_function::LINEAR:
            candidate_index_to_mutate = i % (candidates.size());
            break;

        case selection_function::EXPONENTIAL:
            candidate_index_to_mutate = (i * i) % (candidates.size());
            break;

        default:
            break;
        }

        for (std::size_t j = 0; j < amount_gens_mutations; j++)
        {
            std::uniform_int_distribution<std::size_t> distrib(0, candidates[i].get_function().size() - 1);
            std::size_t mutation_point = distrib(gen);

            candidates[candidate_index_to_mutate].get_function()[mutation_point] =
                (candidates[candidate_index_to_mutate].get_function()[mutation_point] == '0') ? '1' : '0';
        }
    }
}

void GeneticAlgorithm::do_selection(
    std::vector<Candidate> &candidates,
    std::size_t population
)
{
    std::sort(candidates.begin(), candidates.end(), [](const Candidate &lhs, const Candidate &rhs)
              { return lhs.get_quality() < rhs.get_quality(); });

    candidates.resize(population);
}
