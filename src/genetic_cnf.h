#ifndef GENETIC_CHF_H
#define GENETIC_CHF_H

#include "cnf.h"

typedef enum 
{
    RANDOM,
    LINEAR,
    EXPONENTIAL
} selection_function;


// функция качества для кандидата на решение 
double quality_function(const char* cnf, const char* candidate);

// скрещивание
Candidate* hybridization(Candidate* candidates, size_t amount_hybridization, selection_function sf);

// мутиация
void mutate(Candidate* candidates, size_t amount_mutations, selection_function sf, size_t amount_gens_mutatuions);

// отбор
Candidate* selection(Candidate* candidates);

// генетический алгоритм
void genetic_algorithm();


#endif // GENETIC_CHF_H