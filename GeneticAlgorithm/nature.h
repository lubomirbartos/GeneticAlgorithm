
#ifndef NATURE_H
#define NATURE_H
#include "structures.h"

void create_initial_population(creature ** population, int * count_of_creatures, environment * env);
void cross_binary_and_append(int f_gene, int m_gene, gene * offspring_gene_int);
void cross_real_and_append(float f_gene, float m_gene, gene * offspring_gene_real);
void create_random_gene(gene * gene, environment *env);
void mutate_population(creature * population, int mutation_percentage, int population_count, environment * env);
void breed_offspring(creature * population, int mother_index, int father_index, environment * env);
void mutate_creature(creature * creature, environment * env);
void kill_creature(creature * creature);
void test_creature(creature * creature, environment * env);
void mating_time(creature ** population, int * population_count, environment * env);
void cross_gene(gene * mother_gene, gene * father_gene, gene * offspring_gene[], environment * env);
void dying_time(creature ** population, int * population_count);
void copy_gene(gene * to, gene * from, environment * env);
void kill_all(creature * population);
void evolve(creature ** population, int * population_count, int mutation_percentage, environment * env, int last_generation);
void life(int count_of_generations, int mutation_percentage, environment * env);

creature * create_creature(environment * env);

#endif
