#include "genetic_cnf.h"
#include "utils.h"

#include <math.h>

#define EPS 1e-10

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

char* genetic_algorithm()
{
    size_t true_candidate_counter = 0;
    while(fabs(generated_candidates[true_candidate_counter].quality - 1.0) > EPS)
    {
        /*  
         *  так как популяция уже на этом этапе 
         *  будет создана первым делом посчитаем
         *  функцию качества для кандидатов
        */

        for (size_t i = 0; i < AMOUNT_INDIVIDUALS; i++)
        {
            generated_candidates[i].quality = quality_function(generated_cnf->str, generated_candidates[i].function);
        }

        /*
         *  скрещивание
         *
         * 
        */

        /*
         *  мутация
         *
         * 
        */

        true_candidate_counter++;
    }

    // возврат истинного решения 
    return generated_candidates[true_candidate_counter].function;
}
