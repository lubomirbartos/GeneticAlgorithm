#ifndef APP_H
#define APP_H
#include "structures.h"


double evaluate(char *result);
void write_to_file();
void evolve(jedinec *population, int *population_count, int mutation_percentage, environment *env);
void life(int count_of_generations, int mutation_percentage, environment *env);

#endif
