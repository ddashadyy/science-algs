#include "genetic_cnf.h"
#include "utils.h"

#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <ctype.h>


bool eval_disjunct(const char* disjunct, const char* function)
{
    if (!disjunct || !function || *disjunct == '\0') {
        return false;
    }

    const char* p = disjunct;
    while (*p) 
    {
        while (*p == ' ') p++;
        if (*p == '\0') break;

        bool negated = false;
        char var_char = *p;

        if (var_char == '!') 
        {
            negated = true;
            p++;
            var_char = *p;
        }

        if (var_char != 'x' && !isdigit(var_char)) 
        {
            p++;
            continue; 
        }

        int var_index = 0;
        if (var_char == 'x') 
        {
            p++;
            if (!isdigit(*p)) 
            {
                continue; 
            }
            var_index = *p - '0' - 1; 
        } 
        else 
        {
            var_index = var_char - '0' - 1; 
        }

        if (var_index < 0 || var_index >= strlen(function)) 
        {
            p++;
            continue;
        }

        bool var_value = (function[var_index] == '1');

        if (negated) 
        {
            var_value = !var_value;
        }

        if (var_value) 
        {
            return true;
        }

        p++;
    }

    return false;
}


double quality_function(char* cnf, const char* candidate)
{

    /*
     *  чтобы мне вернуть функцию качества
     *  мне нужно знать размер массива строк 
     *  с дизъюнкциями
     *  результат = кол-во скобок == 1 / кол-во всех скобок
     *  думаю нужна структура, хранящая размер
    */
    size_t quality_counter = 0;

    Splitted_CNF* splitted = split(cnf);
    for (size_t i = 0; i < splitted->amount_disjunctions; i++)
    {
        /*
         *  вычисляем дизъюнкт и возращаем результат
         *  для каждой скобки
        */

        if (eval_disjunct(splitted->splitted_cnf[i], candidate))
            quality_counter++;

    }

    size_t amount_disj = splitted->amount_disjunctions;

    for (size_t i = 0 ; i < splitted->amount_disjunctions; i++)
        free(splitted->splitted_cnf[i]);
    free(splitted->splitted_cnf);
    free(splitted);

    return (double) quality_counter / amount_disj;
}

void hybridization(size_t amount_hybridization, selection_function sf, size_t population)
{
    generated_candidates = (Candidate*) realloc(generated_candidates, (population + amount_hybridization / 2) * sizeof(Candidate));
   
    for (size_t i = 1; i <= amount_hybridization; i += 2)
    {
        size_t selection_index_of_first_parent = 0;
        size_t selection_index_of_second_parent = 0;

        switch (sf)
        {
        case RANDOM:
            selection_index_of_first_parent = rand() % population;
            selection_index_of_second_parent = rand() % population;
            break;
    
        case LINEAR:
            selection_index_of_first_parent = (i - 1) % population;
            selection_index_of_second_parent = i % population;
            break;

        case EXPONENTIAL:
            selection_index_of_first_parent = (i * i) % population;
            selection_index_of_second_parent = ((i - 1) * (i - 1)) % population;
            break;
        
        default:
            break;
        }

        size_t length = generated_candidates[i].size;
        int point = rand() % (length - 1) + 1;

        char* child = (char*) malloc(length + 1);

        strncpy(child, generated_candidates[selection_index_of_first_parent].function, point);
        strcpy(child + point, generated_candidates[selection_index_of_second_parent].function);
        child[length] = '\0';

        size_t child_index = population + i / 2;

        generated_candidates[child_index].size = length;
        strcpy(generated_candidates[child_index].function, child);

        free(child);
    }
}

void mutate(size_t amount_mutations, selection_function sf, size_t amount_gens_mutatuions)
{
    for (size_t i = 0; i < amount_mutations; i++)
    {
        size_t candidate_index_to_mutate = 0;
        size_t length = generated_candidates[i].size;

        switch (sf)
        {
        case RANDOM:
            candidate_index_to_mutate = rand() % length;
            break;
        
        case LINEAR:
            candidate_index_to_mutate = i % length;
        
        case EXPONENTIAL:
            candidate_index_to_mutate = (i * i) % length;
        default:
            break;
        }

        char* mutated_function = (char*) malloc(length + 1);
        for (size_t j = 0; j < amount_gens_mutatuions; j++)
        {
            int mutation_point = rand() % length;
            mutated_function[mutation_point] = (mutated_function[mutation_point] == '0') ? '1' : '0';
        }

        strcpy(generated_candidates[candidate_index_to_mutate].function, mutated_function);
        free(mutated_function);
    }
}

int compare_candidates(const void *lhs, const void *rhs)
{
    Candidate arg1 = *(Candidate*) lhs;
    Candidate arg2 = *(Candidate*) rhs;

    if (arg1.quality > arg2.quality) return 1;
    if (arg1.quality < arg2.quality) return -1;
    return 0;
}

void selection(size_t population_size, size_t amount_hybridization)
{
    qsort(generated_candidates, population_size + amount_hybridization, sizeof(Candidate), compare_candidates);
    generated_candidates = (Candidate*) realloc(generated_candidates, population_size);
}

char* genetic_algorithm(genetic_alg_params* params)
{
    size_t true_candidate_counter = 0;
    for (size_t i = 0; i < params->iterations; i++)
    {
        /*  
         *  так как популяция уже на этом этапе 
         *  будет создана первым делом посчитаем
         *  функцию качества для кандидатов
        */

        for (size_t j = 0; j < params->population; j++)
            generated_candidates[j].quality = quality_function(generated_cnf->str, generated_candidates[j].function);
        
        /*
         *  скрещивание
        */
        
        hybridization(params->amount_hybridizations, params->sf, params->population);

        /*
         *  мутация
        */

        mutate(params->amount_mutations, params->sf, params->amount_gens_mutations);

        /*
         *  отбор
        */

        selection(params->population, params->amount_hybridizations);

        true_candidate_counter++;

        for (size_t j = 0; j < params->population; j++)
            if (fabs(generated_candidates[j].quality - 1.0) < DBL_EPSILON * fabs(generated_candidates[j].quality + 1.0))
                return generated_candidates[j].function;
    }

    printf("%llu\n");
    return "there is no solution\0";
}
