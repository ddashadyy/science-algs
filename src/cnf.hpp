#ifndef CNF_HPP
#define CNF_HPP

#include <string>
#include <vector>

enum class selection_function;
class CNF;
class Candidate;
class Candidates;

enum class selection_function
{
    RANDOM,
    LINEAR,
    EXPONENTIAL
};

class CNF
{
public:
    CNF();
    CNF(std::size_t anount_variables);
    ~CNF();

    std::string& get_cnf();
    std::vector<std::string> split_cnf() const;

private:
    std::string generate_random_cnf(std::size_t variable_number, std::size_t clause_length, std::size_t max_clause_length) const;
    std::string trim_and_clean(const std::string& s) const;

    std::string str;
};

class Candidate
{
public:
    Candidate();
    Candidate(std::size_t function_length);
    Candidate(const std::string& str);
    ~Candidate();

    std::string& get_function();
    double get_quality() const;
    void evaluate_quality_function(const CNF& cnf);

private:
    std::string generate_random_boolean_function(std::size_t length);
    bool eval_disjunct(const std::string& disjunct) const;

    std::string function;
    double quality;
};

class Candidates
{
public:
    Candidates();
    Candidates(std::size_t amount_candidates, std::size_t function_length);
    ~Candidates();

    std::vector<Candidate>& get_candidates();

private:
    std::vector<Candidate> solution_candidates;
};

#endif // CNF_HPP