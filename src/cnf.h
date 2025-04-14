#ifndef CNF_H
#define CNF_H

#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>

// пока что одна из идей это создать массив на 100 кнф по этой структуре 
// и работать с ней уже при реализации алгоритмов
typedef struct CNF_t {
    char* cnf_str;
    char* boolean_function;
} CNF;

extern CNF* cnfs;

void init_random(); 
char* generate_random_boolean_function(size_t n);
char* generate_cnf_from_function(char* function);
void write_cnf_in_file(size_t n);


#endif