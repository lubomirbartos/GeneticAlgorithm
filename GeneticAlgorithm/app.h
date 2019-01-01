#ifndef APP_H
#define APP_H
#include "structures.h"

void evolve(jedinec **population, int *population_count, int mutation_percentage, environment *env, int last_generation);
void life(int count_of_generations, int mutation_percentage, environment *env);

#endif
