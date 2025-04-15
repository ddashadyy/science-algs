#ifndef CNF_H
#define CNF_H

#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>


void init_random(); 

char* generate_random_boolean_function(size_t n);
char* generate_cnf(size_t num_vars, size_t num_clauses, size_t max_clause_len);

void write_cnf_in_file(size_t n);
void write_candidates_in_file(size_t n);


#endif // CNF_H