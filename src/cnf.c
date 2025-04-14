#include "cnf.h"

void init_random() 
{ 
    srand(time(NULL)); 
}

const char* generate_random_cnf(size_t num_vars, size_t num_clauses, size_t max_clause_len) 
{
    char variables[num_vars][10];
    
    for (size_t i = 0; i < num_vars; i++) 
        sprintf(variables[i], "x%d", i + 1);
    
    static char cnf_str[5000] = "";
    cnf_str[0] = '\0';

    for (size_t i = 0; i < num_clauses; i++) 
    {
        strcat(cnf_str, "(");
        size_t clause_len = 1 + rand() % max_clause_len;
        size_t used_vars[num_vars];
        memset(used_vars, 0, sizeof(used_vars));

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
                strcat(cnf_str, "~");
            
            strcat(cnf_str, variables[var_idx]);

            if (j < clause_len - 1) 
                strcat(cnf_str, " | ");
            
        }
        strcat(cnf_str, ")");
        if (i < num_clauses - 1) 
            strcat(cnf_str, " & ");
        
    }

    return cnf_str;
}
