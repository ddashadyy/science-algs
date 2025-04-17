#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h>

typedef struct 
{
    char** splitted_cnf;
    size_t amount_disjunctions;
} Splitted_CNF;

Splitted_CNF* split(char* str);

#endif // UTILS_H