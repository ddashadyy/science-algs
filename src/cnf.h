#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>

#define MAX_VARS 10
#define MAX_CLAUSES 20
#define MAX_LITERALS 10

typedef struct {
    size_t var_index;      // Индекс переменной (начинается с 0)
    bool negated;          // Флаг отрицания
} Literal;

typedef struct {
    Literal* literals;     // Массив литералов
    size_t count;          // Количество литералов
} Clause;

typedef struct {
    Clause* clauses;       // Массив дизъюнктов
    size_t count;          // Количество дизъюнктов
    size_t var_count;      // Количество уникальных переменных
} CNF;

void init_random(); 
const char* generate_random_cnf(size_t num_vars, size_t num_clauses, size_t max_clause_len);

