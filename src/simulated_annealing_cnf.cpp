#include "simulated_annealing_cnf.hpp"


#include <random>
#include <iostream>
#include <stdexcept>

constexpr double EPSILON = std::numeric_limits<double>::epsilon() * 10;


SaAlgorithm::SaAlgorithm(
    const CNF& cnf,
    const Candidate& candidate
) : cnf_(cnf), candidate_(candidate) {}


SaAlgorithm::~SaAlgorithm() = default;

Candidate SaAlgorithm::execute(double initial_temp, double min_temp, double cooling_rate, cooling_type ct, std::size_t iterations)
{
    quality_history_.clear(); 
    Candidate best_solution{candidate_.get_function()};

    double temp = initial_temp;

    double current_energy = eval_energy(candidate_);
    std::cout << "current energy: " << current_energy << '\n';
    std::cout << "current function: " << candidate_.get_function() << '\n';
    double best_energy = current_energy;

    quality_history_.emplace_back(temp); 

    for (size_t i = 0; (i < iterations) || (temp > min_temp); ++i) 
    {
        if (std::fabs(current_energy - 1.0) < EPSILON) break; 

        Candidate neighbor = get_neighbor();
        double neighbor_energy = eval_energy(neighbor);

        if (accept_solution(current_energy, neighbor_energy, temp)) 
        {
            candidate_ = neighbor;
            current_energy = neighbor_energy;

            if (current_energy < best_energy) 
            {
                best_solution = candidate_;
                best_energy = current_energy;
            }
        }

        quality_history_.emplace_back(temp);
        temp = cool(initial_temp, min_temp, cooling_rate, ct, i, iterations);
    }

    return best_solution;
}

std::vector<double> SaAlgorithm::get_quality_history() const
{
    return quality_history_;
}

double SaAlgorithm::eval_energy(Candidate& candidate) const
{
    candidate.evaluate_quality_function(cnf_);
    return 1.0 - candidate.get_quality();
}

Candidate SaAlgorithm::get_neighbor() const
{
    Candidate neighbor = candidate_;
    std::string neighbor_func = neighbor.get_function();
    
    if (neighbor_func.empty()) 
        throw std::runtime_error("Current solution function is empty");
    

    /*static*/ std::random_device rd;
    /*static*/ std::mt19937 rng(rd());
    
    std::uniform_int_distribution<size_t> dist(0, neighbor_func.size() - 1);
    std::size_t flip_pos = dist(rng);
    
    neighbor_func[flip_pos] = (neighbor_func[flip_pos] == '1') ? '0' : '1';
    
    neighbor = Candidate(neighbor_func);
    
    return neighbor;
}

bool SaAlgorithm::accept_solution(
    double current_enegry, double new_energy, double temp
) const
{
    /*static*/ std::random_device rd;  
    /*static*/ std::mt19937 rng(rd()); 

    if (new_energy < current_enegry) return true;
        
    double delta = new_energy - current_enegry;
    double probability = exp(-delta / temp);
    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
    return prob_dist(rng) < probability;
}

double SaAlgorithm::cool(
    double initial_temp, 
    double min_temp, 
    double cooling_rate, 
    cooling_type ct, 
    size_t iteration, 
    size_t max_iterations
) const 
{
    switch (ct) 
    {
        case cooling_type::LINEAR:
            return initial_temp - (initial_temp - min_temp) * iteration / max_iterations;

        case cooling_type::EXPONENTIAL:
            return initial_temp * pow(cooling_rate, iteration);

        default:
            throw std::invalid_argument("Unknown cooling type");
    }
}
