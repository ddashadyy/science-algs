#ifndef GENETIC_CHF_H
#define GENETIC_CHF_H

#include "cnf.h"



bool eval_disjunct(const char* disjunct, const char* function);
// функция качества для кандидата на решение 
double quality_function(char* cnf, const char* candidate);

// скрещивание
void hybridization(size_t amount_hybridization, selection_function sf, size_t population);

// мутиация
void mutate(size_t amount_mutations, selection_function sf, size_t amount_gens_mutatuions);

// компаратор для отбора кандидата
int compare_candidates(const void* lhs, const void* rhs);

// отбор
void selection(size_t population_size, size_t amount_hybridization);

// генетический алгоритм
char* genetic_algorithm(genetic_alg_params* params);


#endif // GENETIC_CHF_H