#include "cnf.h"

#include <stdlib.h>

uint8_t** generete_truth_table(size_t n)
{
    size_t rows = 1 << n;

    uint8_t** truth_table = (uint8_t**) malloc(n * sizeof(uint8_t*));
    if (!truth_table) return NULL;

    for (size_t i = 0; i < n; i++)
    {
        truth_table[i] = (uint8_t*) malloc(rows * sizeof(uint8_t));
        if (!truth_table[i])
        {
            for (size_t j = 0; j < i; j++) free(truth_table[j]);
            free(truth_table);

            return NULL;
        }

        for (size_t j = 0; j < rows; j++)
        {
            truth_table[i][j] = ((i >> j) & 1);
        }
    }

    return truth_table;
}

void free_two_dimensioal_array(uint8_t** array, size_t rows)
{
    if (!array) return;

    for (size_t i = 0; i < rows; i++) 
        free(array[i]);
    
    free(array);
}
