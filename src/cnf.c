#include "cnf.h"

#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <windows.h>



CNF* generated_cnf = NULL;
Candidate* generated_candidates = NULL;


void init_random() 
{ 
    srand(time(NULL)); 
}

char* generate_random_boolean_function(size_t n) 
{
    if (n == 0 || n > 64) return NULL; 

    size_t size = 1ULL << n;
    
    char* function = (char*)malloc(size + 1);
    if (!function) return NULL;
    
    static int initialized = 0;
    if (!initialized) 
    {
        srand(time(NULL));
        initialized = 1;
    }
    
    for (size_t i = 0; i < size; i++) 
        function[i] = (rand() % 2) ? '1' : '0';    
    function[size] = '\0'; 

    return function;
}

char* generate_cnf(size_t num_vars, size_t num_clauses, size_t max_clause_len)
{
    char** variables = (char**)malloc(num_vars * sizeof(char*));
    if (!variables) return NULL;
    
    for (size_t i = 0; i < num_vars; i++) {
        variables[i] = (char*)malloc(10 * sizeof(char));
        if (!variables[i]) 
        {    
            for (size_t j = 0; j < i; j++) 
                free(variables[j]);
            
            free(variables);
            return NULL;
        }
        sprintf(variables[i], "x%zu", i + 1);
    }
    
    size_t buf_size = 1024;

    char* cnf_str = (char*)malloc(buf_size * sizeof(char));
    if (!cnf_str) 
    {
        for (size_t i = 0; i < num_vars; i++) 
            free(variables[i]);
        
        free(variables);
        return NULL;
    }
    cnf_str[0] = '\0';
    
    for (size_t i = 0; i < num_clauses; i++) 
    {    
        size_t needed_space = strlen(cnf_str) + max_clause_len * 15 + 10;
        if (needed_space >= buf_size) 
        {
            buf_size = needed_space * 2;

            char* new_buf = (char*)realloc(cnf_str, buf_size);
            if (!new_buf) 
            {
                free(cnf_str);
                for (size_t j = 0; j < num_vars; j++) 
                    free(variables[j]);
                
                free(variables);
                return NULL;
            }
            cnf_str = new_buf;
        }
        
        strcat(cnf_str, "(");
        size_t clause_len = 1 + rand() % max_clause_len;
        size_t* used_vars = (size_t*)calloc(num_vars, sizeof(size_t));
        if (!used_vars) 
        {
            free(cnf_str);
            for (size_t j = 0; j < num_vars; j++) 
                free(variables[j]);
            
            free(variables);
            return NULL;
        }

        for (size_t j = 0; j < clause_len; j++) 
        {
            size_t var_idx;
            size_t attempts = 0;
            do 
            {
                var_idx = rand() % num_vars;
                attempts++;
                
                if (attempts > num_vars * 2) 
                {
                    clause_len = j;
                    break;
                }
            } while (used_vars[var_idx] && clause_len > 1);
            
            if (j >= clause_len) continue;
            used_vars[var_idx] = 1;

            if (rand() % 3 == 0) 
                strcat(cnf_str, "!");
            
            strcat(cnf_str, variables[var_idx]);

            if (j < clause_len - 1) 
                strcat(cnf_str, " | ");
            
        }
        free(used_vars);
        
        strcat(cnf_str, ")");
        if (i < num_clauses - 1) 
            strcat(cnf_str, " & ");
        
    }
    
    for (size_t i = 0; i < num_vars; i++) 
        free(variables[i]);
    
    free(variables);
    
    char* result = realloc(cnf_str, strlen(cnf_str) + 1);
    return result ? result : cnf_str;
}

void write_cnf_in_file(size_t n)
{
    FILE* cnf_file = fopen("cnf.txt", "w");
    if (!cnf_file)
    {
        printf("file not created\n");
        return;
    }

    char* cnf = generate_cnf(n, n + 2, n);

    generated_cnf = (CNF*) malloc(sizeof(CNF));

    // заполнение КНФ
    generated_cnf->str = cnf;
    generated_cnf->size = strlen(cnf);

    fputs(cnf, cnf_file);
    fputc('\n', cnf_file);

    free(cnf);
    fclose(cnf_file);

}

void write_candidates_in_file(size_t n)
{
    FILE* candidates = fopen("candidates.txt", "w+");
    if (!candidates)
    {
        printf("file not created\n");
        return;
    }

    generated_candidates = (Candidate*) malloc(AMOUNT_INDIVIDUALS * sizeof(Candidate));

    for (size_t i = 0; i < AMOUNT_INDIVIDUALS; i++)
    {
        char* candidate = generate_random_boolean_function(n);

        // заполнение кандидатов
        generated_candidates[i].function = candidate;
        generated_candidates[i].quality = 0.0;
        generated_candidates[i].size = strlen(candidate);

        fputs(candidate, candidates);
        fputc('\n', candidates);

        free(candidate);
    }
    
    fclose(candidates);
}
