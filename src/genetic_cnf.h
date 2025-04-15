#ifndef GENETIC_CHF_H
#define GENETIC_CHF_H


#include "cnf.h"

typedef struct {
    char* function;
    double quality;
} Candidate;


// функция для загрузки КНФ из файла
char* load_cnf_from_file(const char* file_name);

// функция для загрузки кандидатов из файла
Candidate* load_candidates_from_file(const char* file_name);

// функция качества для кандидата на решение 
double quality_function(const char* cnf, const char* candidate);

// скрещивание
Candidate* hybridization(Candidate* candidates);

// мутиация
void mutate(Candidate* candidates);

// отбор
Candidate* selection(Candidate* candidates);

// генетический алгоритм
void genetic_algorithm();


#endif // GENETIC_CHF_H