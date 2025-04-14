#include "cnf.h"

#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#define AMOUNT_INDIVIDUALS 100

CNF* cnfs = NULL;

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

char* generate_cnf_from_function(char* function) 
{
    const size_t size = strlen(function);
    if (size == 0 || (size & (size - 1)) != 0) 
        return NULL; 
    
    const size_t n = (size_t)log2(size);
    if (n > 32) return NULL; 
    
    size_t max_cnf_len = 0;
    size_t zero_count = 0;
    
    for (size_t i = 0; i < size; i++) 
    {
        if (function[i] == '0') 
        {
            zero_count++;
            max_cnf_len += 2 + n * 4 + (n - 1) * 3;
        }
    }
    
    if (zero_count > 1) 
        max_cnf_len += (zero_count - 1) * 3;
    
    char* cnf_str = (char*)malloc(max_cnf_len + 1);
    if (!cnf_str) return NULL;
    cnf_str[0] = '\0';
    
    size_t current_pos = 0;
    
    for (size_t i = 0; i < size; i++) 
    {
        if (function[i] == '0') 
        {
            
            strcpy(cnf_str + current_pos, "(");
            current_pos += 1;
            
            for (size_t j = 0; j < n; j++) 
            {
                if ((i & (1 << j)))
                {
                    strcpy(cnf_str + current_pos, "!x");
                    current_pos += 2;
                } 
                else 
                {
                    strcpy(cnf_str + current_pos, "x");
                    current_pos += 1;
                }
                
                int written = sprintf(cnf_str + current_pos, "%zu", j + 1);
                current_pos += written;
                
                if (j < n - 1) 
                {
                    strcpy(cnf_str + current_pos, " | ");
                    current_pos += 3;
                }
            }
            
            strcpy(cnf_str + current_pos, ")");
            current_pos += 1;
            
            if (i < size - 1 && strchr(function + i + 1, '0') != NULL) 
            {
                strcpy(cnf_str + current_pos, " & ");
                current_pos += 3;
            }
        }
    }
    
    return cnf_str;
}

void write_cnf_in_file(size_t n)
{
    FILE* cnf_file = fopen("cnf_file.txt", "w+");
    if (!cnf_file) 
    {
        printf("file not created\n");
        return;
    }

    cnfs = (CNF*) malloc(AMOUNT_INDIVIDUALS * sizeof(CNF));   

    for (size_t i = 0; i < AMOUNT_INDIVIDUALS; i++)
    {
        char* function = generate_random_boolean_function(n);
        char* cnf = generate_cnf_from_function(function);

        cnfs[i].boolean_function = function;
        cnfs[i].cnf_str = cnf;
        
        for (size_t i = 0; i < strlen(cnf); i++)
        fputc(cnf[i], cnf_file);

        fputc('\n', cnf_file);

        free(function);
        free(cnf);
    }

    fclose(cnf_file);
}
