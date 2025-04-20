#include "cnf.hpp"

#include <fstream>
#include <sstream>
#include <random>

CNF::CNF(std::size_t amount_variables)
{
    std::ofstream cnf_out;
    cnf_out.open("cnf.txt");

    if (cnf_out.is_open())
    {
        auto cnf = generate_random_cnf(amount_variables, amount_variables + 10, amount_variables + 5);
        cnf_out << cnf;

        this->str = cnf;
    }

    cnf_out.close();
}

CNF::CNF() = default;

CNF::~CNF() = default;

std::string& CNF::get_cnf()
{
    return this->str;
}

std::vector<std::string> CNF::split_cnf() const
{
    std::vector<std::string> result;
    if (this->str.empty())
        return std::vector<std::string>{"empty"};

    const char delimiter = '&';
    std::istringstream iss(this->str);
    std::string token;

    while (std::getline(iss, token, delimiter))
    {
        std::string cleaned = trim_and_clean(token);
        if (!cleaned.empty())
            result.push_back(cleaned);
    }

    if (result.empty())
    {
        std::string cleaned = trim_and_clean(this->str);
        if (!cleaned.empty())
            result.push_back(cleaned);
    }

    return result;
}

std::string CNF::generate_random_cnf(std::size_t variable_number,
                                     std::size_t clause_length,
                                     std::size_t max_clause_length) const
{
    if (variable_number == 0 || clause_length == 0 || max_clause_length == 0)
        return "";

    // Инициализация генератора случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> clause_len_dist(1, max_clause_length);
    std::uniform_int_distribution<size_t> var_dist(0, variable_number - 1);
    std::bernoulli_distribution neg_dist(0.33); // Вероятность отрицания ~1/3

    std::vector<std::string> variables;
    for (size_t i = 0; i < variable_number; ++i)
        variables.push_back("x" + std::to_string(i + 1));

    std::string cnf_str;

    for (size_t i = 0; i < clause_length; ++i)
    {
        cnf_str += "(";

        size_t clause_len = clause_len_dist(gen);
        std::vector<bool> used_vars(variable_number, false);
        size_t vars_in_clause = 0;

        for (size_t j = 0; j < clause_len; ++j)
        {
            size_t var_idx;
            size_t attempts = 0;

            do
            {
                var_idx = var_dist(gen);
                attempts++;

                if (attempts > variable_number * 2)
                    break;

            } while (used_vars[var_idx] && clause_len > 1);

            if (attempts > variable_number * 2)
                break;

            used_vars[var_idx] = true;
            vars_in_clause++;

            if (neg_dist(gen))
                cnf_str += "!";

            cnf_str += variables[var_idx];

            if (j < clause_len - 1 && vars_in_clause < clause_len)
                cnf_str += " | ";
        }

        if (cnf_str.size() >= 3 && cnf_str.substr(cnf_str.size() - 3) == " | ")
            cnf_str.erase(cnf_str.size() - 3);

        cnf_str += ")";

        if (i < clause_length - 1)
            cnf_str += " & ";
    }

    return cnf_str;
}

std::string CNF::trim_and_clean(const std::string& s) const
{
    if (s.empty())
        return s;

    size_t start = s.find_first_not_of(" \t()");
    if (start == std::string::npos)
        return "";

    size_t end = s.find_last_not_of(" \t()");
    return s.substr(start, end - start + 1);
}

Candidate::Candidate() = default;

Candidate::Candidate(std::size_t function_length) : quality(0.0)
{
    this->function = generate_random_boolean_function(function_length);
}

Candidate::Candidate(const std::string& str)
{
    this->function = str;
}

Candidate::~Candidate() = default;

void Candidate::evaluate_quality_function(const CNF& cnf)
{
    auto disjuncts = cnf.split_cnf();
    size_t quality_counter = 0;

    for (const auto &disjunct : disjuncts)
        if (eval_disjunct(disjunct))
            ++quality_counter;

    this->quality = static_cast<double>(quality_counter) / disjuncts.size();
}

bool Candidate::eval_disjunct(const std::string& disjunct) const
{
    if (disjunct.empty() || this->function.empty())
        return false;

    size_t pos = 0;
    while (pos < disjunct.length())
    {
        while (pos < disjunct.length() && disjunct[pos] == ' ')
            pos++;
        if (pos >= disjunct.length())
            break;

        bool negated = false;
        char var_char = disjunct[pos];

        if (var_char == '!')
        {
            negated = true;
            pos++;
            if (pos >= disjunct.length())
                break;
            var_char = disjunct[pos];
        }

        if (var_char != 'x' && !isdigit(var_char))
        {
            pos++;
            continue;
        }

        int var_index = 0;
        if (var_char == 'x')
        {
            pos++;
            if (pos >= disjunct.length() || !isdigit(disjunct[pos]))
                continue;

            var_index = disjunct[pos] - '0' - 1;
        }
        else
            var_index = var_char - '0' - 1;

        if (var_index < 0 || var_index >= static_cast<int>(this->function.length()))
        {
            pos++;
            continue;
        }

        bool var_value = (this->function[var_index] == '1');

        if (negated)
            var_value = !var_value;
        if (var_value)
            return true;

        pos++;
    }

    return false;
}

std::string Candidate::generate_random_boolean_function(std::size_t length)
{
    if (length == 0 || length > 64)
        throw std::invalid_argument("Function length must be between 1 and 64");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution distrib(0.5); // 50% chance for 0 or 1

    std::string function;
    function.resize(length);

    for (size_t i = 0; i < length; ++i)
        function[i] = distrib(gen) ? '1' : '0';

    return function;
}

std::string& Candidate::get_function()
{
    return this->function;
}

double Candidate::get_quality() const
{
    return this->quality;
}

Candidates::Candidates(std::size_t amount_candidates, std::size_t function_length)
{
    std::ofstream candidates_out("candidates.txt");
    for (std::size_t i = 0; i < amount_candidates; i++)
    {
        solution_candidates.emplace_back(Candidate(function_length));
        if (candidates_out.is_open())
            candidates_out << solution_candidates[i].get_function() << '\n';
    }
}

Candidates::Candidates() = default;

Candidates::~Candidates() = default;

std::vector<Candidate>& Candidates::get_candidates()
{
    return this->solution_candidates;
}
