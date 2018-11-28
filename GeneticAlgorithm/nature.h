
#ifndef NATURE_H
#define NATURE_H
#include "structures.h"


void dying_time(jedinec *population, int *population_count);
void create_initial_population(jedinec *population, int count_of_creatures, environment *env);
jedinec *create_creature(environment *env);
void create_random_gene(char **gene, environment *env);
void mating_time(jedinec *population, int *population_count, int mutation_percentage, environment *env);
jedinec *get_creature_by_number(jedinec *population, int index);
void kill_creature(jedinec *creature, int index);
void test_creature(jedinec * creature, float *result, environment *env);
void breed_offspring(jedinec *population, int mother_index, int father_index, environment *env, int mutation_percentage);
jedinec *get_last_creature_in_list(jedinec *population);
void cross_gene(char *mother_gene, char *father_gene, char **offspring_gene, environment *env, int mutation_percentage);
void cross_binary_and_append(int f_gene, int m_gene, char **offspring_gene, int mutation_percentage);
void cross_real_and_append(float f_gene, float m_gene, char **offspring_gene, int mutation_percentage);


#define VARIABLE_TYPE_INTEGER 'Z'
#define VARIABLE_TYPE_REAL 'R'

#endif