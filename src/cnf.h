#include <stddef.h>
#include <inttypes.h>


// решили пока что представлять кнф в виде двумерного массиво
typedef struct cnf {
    uint8_t** disjuncts;
    size_t brackets_amount;
    size_t disjuncts_amount;
} cnf;


uint8_t** generete_truth_table(size_t n);
void free_two_dimensioal_array(uint8_t** array, size_t rows);
// uint8_t** generate_random_disjuncts_matrix(size_t disjuncts_amount);
// cnf* create_cnf(size_t disjuncts_amount);
