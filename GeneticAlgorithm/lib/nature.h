
#ifndef NATURE_H
#define NATURE_H
#include "structures.h"

void dying_time(creature **population, int *population_count);
void kill_creature(creature *creature);
void kill_all(creature *population);
void evolve(creature **population, int *population_count, int mutation_percentage, environment *env, int last_generation);
void life(int count_of_generations, int mutation_percentage, environment *env);
void get_average_fitness(creature *population, int population_count, float *average_fitness);


int get_valuable_creature_index(creature *population, int last_creature_index);

void mating_time(creature **population, int *population_count, environment *env);
void create_initial_population(creature **population, int *count_of_creatures, environment *env);
void cross_gene(gene *mother_gene, gene *father_gene, gene *offspring_gene[], environment *env);
void cross_binary_and_append(int f_gene, int m_gene, gene *offspring_gene_int);
void cross_real_and_append(float f_gene, float m_gene, gene *offspring_gene_real);
creature *create_creature(environment *env);
void create_random_gene(gene *gene, environment *env);
void test_creature(creature * creature, environment *env);
void breed_offspring(creature *population, int mother_index, int father_index, environment *env);
creature *get_last_creature_in_list(creature *population);
creature *get_creature_by_number(creature *population, int index);
void copy_gene(gene *to, gene *from, environment *env);
void print_population(creature *population);
void log_fittest(creature *population,int generation_number, environment *env);
void mutate_population(creature *population, int mutation_percentage, int population_count, environment * env);
void mutate_creature(creature *creature, environment * env);
void remove_alpha_tags(creature *population);
void log_results(creature *individual, environment *env);

#endif
