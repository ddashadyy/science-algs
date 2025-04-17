#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


Splitted_CNF* split(char* str) 
{
    char* str_copy = strdup(str);
    if (!str_copy) return NULL;

    const char a_delim = '&';
    size_t count = 0;
    char* tmp = str_copy;
    char* last_delim = 0;

    /* Count how many elements will be extracted */
    while (*tmp) 
    {
        if (a_delim == *tmp) 
        {
            count++;
            last_delim = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token if needed */
    count += (last_delim && last_delim < (str_copy + strlen(str_copy) - 1));

    Splitted_CNF* result = malloc(sizeof(Splitted_CNF));
    if (!result) 
    {
        free(str_copy);
        return NULL;
    }

    result->splitted_cnf = malloc(sizeof(char*) * (count + 1)); // +1 for NULL terminator
    if (!result->splitted_cnf) 
    {
        free(str_copy);
        free(result);
        return NULL;
    }

    result->amount_disjunctions = count;

    if (count > 0) 
    {
        size_t idx = 0;
        char delim[2] = {a_delim, 0};
        char* token = strtok(str_copy, delim);

        while (token) 
        {
            // Remove parentheses from the beginning and end
            char* start = token;
            while (*start == ' ' || *start == '(') start++; // Skip spaces and opening parentheses
            char* end = start + strlen(start) - 1;
            while (end > start && (*end == ' ' || *end == ')')) end--; // Skip spaces and closing parentheses
            *(end + 1) = '\0'; // Terminate the string
            
            result->splitted_cnf[idx++] = strdup(start);
            token = strtok(NULL, delim);
        }
        result->splitted_cnf[idx] = NULL; // NULL-terminate the array
    } 
    else 
    {
        // If no delimiters found, return the whole string as single element
        char* start = str_copy;
        while (*start == ' ' || *start == '(') start++; // Skip spaces and opening parentheses
        char* end = start + strlen(start) - 1;
        while (end > start && (*end == ' ' || *end == ')')) end--; // Skip spaces and closing parentheses
        *(end + 1) = '\0'; // Terminate the string
        
        result->splitted_cnf[0] = strdup(start);
        result->splitted_cnf[1] = NULL;
        result->amount_disjunctions = 1;
    }

    free(str_copy);
    return result;
}
