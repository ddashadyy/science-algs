#include "genetic_cnf.h"
#include "utils.h"

double quality_function(const char* cnf, const char* candidate)
{

    /*
     *  чтобы мне вернуть функцию качества
     *  мне нужно знать размер массива строк 
     *  с дизъюнкциями
     *  результат = кол-во скобок == 1 / кол-во всех скобок
     *  думаю нужна структура, хранящая размер
    */

    char** splitted = split(cnf);
    for (int i = 0; splitted[i] != NULL; i++)
    {
        
    }
    return 0.0;
}

void genetic_algorithm()
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

}
