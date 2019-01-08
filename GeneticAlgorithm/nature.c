#define _DEFAULT_SOURCE 1
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <float.h>
#include "structures.h"
#include "config.h"
#include "nature.h"


/*
* Evolves a population for number of generations in environment
* Population count and population will be modified
*
* parameter creature   **population          pointer to pointer of population
* parameter int         *population_count    population count
* parameter int          mutation_percentage mutation percentage
* parameter environment *env                 environment, configuration
* parameter int          last_generation     boolean, 1 if it is last generation, 0 if not
*
* return void
*/
void evolve(creature **population, int *population_count, int mutation_percentage, environment *env, int last_generation) {
		int i;
		creature *population_pointer;

		printf("\n*******************************************\n"); /*Comment tag */
		printf("****** GETTING FITNESS FOR CREATURES ******\n"); /*Comment tag */
		printf("*******************************************\n\n"); /*Comment tag */

		population_pointer = *population;

		for(i = 0; i < *population_count; i++) {
				test_creature(population_pointer, env);
				population_pointer = population_pointer->next;
		}

		dying_time(population, population_count);

		assert(*population_count != 0);
		if (!last_generation) {
				mutate_population(*population, mutation_percentage, *population_count, env);
				mating_time(population, population_count, env);
		}
}


/*
* Starts the evolution with given arguments
*
* parameter int count_of_generations count of generations
* parameter int mutation_percentage  mutation percentage
*
* return void
*/
void life(int count_of_generations, int mutation_percentage, environment *env) {
	  creature *population;
		int generation_number;
		int population_count;
		int last_generation;

		population_count = 0;
		last_generation  = 0;

		create_initial_population(&population, &population_count, env);

		for ( generation_number = 0 ; generation_number < count_of_generations ; generation_number++ ) {
				if (generation_number == (count_of_generations - 1)) {
						last_generation = 1;
				}
				printf("\n\n ..........Starting generation %d..........\n", generation_number); /*Comment tag */

				evolve(&population, &population_count, mutation_percentage, env, last_generation);
				remove_alpha_tags(population);
				log_fittest(population, generation_number, env);

				printf("\n\n ...........Ending generation %d...........\n\n", generation_number); /*Comment tag */
		}

		kill_all(population);
}

/*
* Kills creatures that have below average fitness
*
* parameter creature ** population       pointer to pointer of population
* parameter int *       population_count pointer to count of creatures
*
* return void
*/
void dying_time(creature **population, int *population_count) {
		float average_fitness;
		creature *pointer;
		creature *weakling;

		printf("\n*******************************************\n"); /*Comment tag */
		printf("*************** DYING TIME ****************\n"); /*Comment tag */
		printf("*******************************************\n\n"); /*Comment tag */

		pointer = *population;
		get_average_fitness(*population, *population_count, &average_fitness);

		printf("..Average fitness of population:     \t%f \n", average_fitness); /*Comment tag */
		printf("..Population count before the purge: \t%d \n\n", *population_count); /*Comment tag */

		while(pointer) {
				if(pointer->fitness < average_fitness && *population_count > 10 ){
						if (!pointer->previous) { /*first member, we need to change population pointer */
								*population = pointer->next;
						}
						weakling = pointer;
						pointer  = pointer->next;
						kill_creature(weakling);
						(*population_count)--;
						weakling = NULL;
				} else {
						pointer = pointer->next;
				}
		}

		printf("\n..Population count after the purge:  \t%d \n", *population_count); /*Comment tag */
}


/*
* Creates initial population with given count and according to configuration
*
* parameter creature **   population         pointer to pointer of population
* parameter int *         count_of_creatures pointer to count of creatures
* parameter environment * env                configuration for creating creatures
*
* return void
*/
void create_initial_population(creature **population, int *count_of_creatures, environment *env){
		int i;
		creature *creation;
		creature *last;

		*population             = create_creature(env);
		(*population)->previous = NULL;
		(*population)->first    = 1;
		(*population)->last     = 1;
		last                    = *population;

		for (i = 1; i < POPULATION_LIMIT; i++) {
				creation = create_creature(env);
				creation->previous = last;
				creation->first    = 0;
				creation->last     = 1;
				last->last         = 0;
				last->next         = creation;
				last               = creation;
				(*count_of_creatures)++;
		}
}

/*
* Creates creature with random gene according to configuration
*
* parameter (environment *) env configuration for creating creatures
*
* return creature* pointer to created creature
*/
creature *create_creature(environment *env) {
		creature *creation;
		gene *gene;

		gene = calloc(env->count_of_parameters, sizeof(gene));
		if (gene == NULL){
				printf("Malloc failed\n");
				return NULL;
		}

		create_random_gene(gene, env);
		creation = calloc (1, sizeof (creature));
		if (creation == NULL){
				printf("Malloc failed\n");
				return NULL;
		}

		creation->gene = gene;
		if (!creation->gene) {
				printf("Creating gene failed!\n");
				free (creation);
				free (gene);
				return NULL;
		}

		creation->is_alpha = 0;
		creation->fitness  = 0;

		return creation;
}

/*
* Creates random gene according to passed configuration
* and stores it to first argument
*
* parameter  (gene*)         gene  storage for gene
* parameter  (environment *) env   configuration
*
* return void
*/
void create_random_gene(gene *gene, environment *env){
		int i, r;
		float from, to;

		for (i = 0; i < env->count_of_parameters; i++) {
				r = rand();
				sscanf(env->intervals[i], "%f,%f", &from, &to);

				if (*(env->parameters + i) == VARIABLE_TYPE_INTEGER) {
						r %= (int)( to - from );
						gene[i].binary = (int)from + r;
				} else if (*(env->parameters + i) == VARIABLE_TYPE_REAL){
						r %= (int)( to - from );
						gene[i].real  = from + (float) r;
						gene[i].real += (float)rand()/(float)(RAND_MAX);
				}
		}
}

/*
* Creates random pairs that breed new members of population
* Children are appended to end of list
*
* parameter  (creature **)   population       pointer to pointer of population
* parameter  (int *)         population_count population count
* parameter  (environment *) env              configuration
*
* return void
*/
void mating_time(creature **population, int *population_count, environment *env) {
		int mother_index, father_index;
		int last_creature_index;

		printf("\n*******************************************\n"); /*Comment tag */
		printf("************** MATING TIME ****************\n"); /*Comment tag */
		printf("*******************************************\n\n"); /*Comment tag */

		last_creature_index = *population_count -1; /*from 0 until last adult */
		mother_index        = 0;
		father_index        = 0;

		while (*population_count < POPULATION_LIMIT) {
				while (mother_index == father_index) {
						/*strong father and random mother, because if we pick */
						/*only creatures with high fittness, the weaker ones will never breed */
						father_index = get_valuable_creature_index(*population, last_creature_index);
						mother_index = rand() % last_creature_index;
				}

				breed_offspring(*population, father_index, mother_index, env);

				mother_index = 0;
				father_index = 0;

				(*population_count)++;
		}
}

/*
* Makes adjecent creatures point to each other and kills (frees) creature
* memory clean after creature
*
* parameter (creature *) individual pointer to creature to kill
*
* return void
*/
void kill_creature(creature *individual) {
		if (individual->first && individual->last) {
				printf("Killing last creature!\n"); /*Comment tag */
		} else if (individual->first) {
				individual->next->previous = NULL;
				individual->next->first    = 1;
		} else if (individual->last) {
				individual->previous->next = NULL;
				individual->previous->last = 1;
		} else {
				individual->previous->next = individual->next;
				individual->next->previous = individual->previous;
		}

		printf("..Creature with fitness\t %f \tkilled!\n", individual->fitness); /*Comment tag */

		free(individual->gene);
		free(individual);
}

/*
* Writes creature data to meta data file,
* executes executable from command line and gets result,
* which will be stored as fitness of creature
*
* parameter (creature *)    individual pointer to creature to kill
* parameter (environment *) env        configuration
*
* return void
*/
void test_creature(creature * individual, environment *env) {
		FILE *fp;
		char path_buf[BUFSIZE];
		char result[BUFSIZE];
		int count_of_results;

		count_of_results = 0;
		realpath(env->executable, path_buf);
		write_creature_metadata(individual, env);

		/* execute executable from command line */
		if ((fp = popen(path_buf, "r")) == NULL) {
				printf("Error opening pipe!\n");
		}
		/* execute executable from command line */
		while (fgets(result, BUFSIZE, fp) != NULL) {
				count_of_results++;
		}

		if(pclose(fp))  {
				printf("Command not found or exited with error status\n");
		}

		individual->fitness = atof(result);
		log_results(individual, env);
		printf("..Testing results: %f \n", individual->fitness); /*Comment tag */
}

/*
* Creates new offspring with gene based on its parents' genes.
* Pushes new offspring to population list.
*
* parameter (creature *)    population   pointer to population
* parameter int             mother_index index of mother in population
* parameter int             father_index index of father in population
* parameter (environment *) env          configuration
*
* return void
*/
void breed_offspring(creature *population, int mother_index, int father_index, environment *env){
		gene *offspring_gene;
		gene *father_gene;
		gene *mother_gene;
		creature *last_creature;

		offspring_gene = malloc(env->count_of_parameters * sizeof(gene));
		if (!offspring_gene) {
				printf("Malloc failed\n");
				return;
		}

		/* get last creature, we will append offspring next to it */
		last_creature = get_last_creature_in_list(population);
		/* get parents' genes */
		father_gene   = get_creature_by_number(population, father_index)->gene;
		mother_gene   = get_creature_by_number(population, mother_index)->gene;

		cross_gene(mother_gene, father_gene, &offspring_gene, env);

		last_creature->next = (creature*) malloc(sizeof(creature));
		if (last_creature->next == NULL){
				printf("Malloc failed\n");
				return;
		}

		/* Append new offspring to end of list */
		last_creature->next->previous = last_creature;
		last_creature->next->next     = NULL;
		last_creature->next->fitness  = FLT_MIN;
		last_creature->next->is_alpha = 0;
		last_creature->last           = 0;
		last_creature->next->last     = 1;
		last_creature->next->first    = 0;
		last_creature->next->gene     = (gene*) calloc(env->count_of_parameters, sizeof(gene));

		if (last_creature->next->gene == NULL){
				printf("Malloc failed\n");
				return;
		}

		copy_gene(last_creature->next->gene, offspring_gene, env);

		printf("..Parents \t%d and %d \tcreated offspring!\n", father_index, mother_index); /*Comment tag */

		free(offspring_gene);
		last_creature = NULL;
}

/*
* Copies data according to congiration from one gene to another.
*
* parameter (gene *)        to   gene to store copy to
* parameter (gene *)        from gene to copy
* parameter (environment *) env  configuration
*
* return void
*/
void copy_gene(gene *to, gene *from, environment *env){
		int i;

		/* iterate through gene and copy */
		for (i = 0; i < env->count_of_parameters; i++) {
				if (env->parameters[i] == VARIABLE_TYPE_INTEGER) {
						to[i].binary = from[i].binary;
				} else if (env->parameters[i] == VARIABLE_TYPE_REAL) {
						to[i].real = from[i].real;
				}
		}
}

/*
* Kills whole population.
*
* parameter (creature *) population pointer to population
*
* return void
*/
void kill_all(creature *population) {
		creature *individual;
		creature *pointer;
		int some_alive;

		some_alive = 1;
		pointer    = population;

		while (some_alive) {
				individual = pointer;
				if (pointer->next) {
						pointer = pointer->next;
				} else {
						some_alive = 0;
				}

				kill_creature(individual);
		}
}

/*
* Crosses father and mother gene according to configuration an mutates
*
* parameter (gene *)        mother_gene    pointer to mother gene
* parameter (gene *)        father_gene    pointer to father gene
* parameter (gene **)       offspring_gene pointer to pointer to offspring gene
* parameter (environment *) env            configuration
*
* return void
*/
void cross_gene(gene *mother_gene, gene *father_gene, gene **offspring_gene, environment *env){
	char parameter;
	int valid_gene; /*like boolean*/
	int valid_part; /*like boolean*/
	int i;

	do  {
		valid_gene = 1;

		/* Create new gene from parents' genes by crossing */
		for (i = 0; i < env->count_of_parameters; i++) {
			parameter = env->parameters[i]; /* env part of gene */
			if (parameter == VARIABLE_TYPE_INTEGER) {
					cross_binary_and_append(father_gene[i].binary, mother_gene[i].binary, (*offspring_gene) + i);
					valid_part = is_valid_int(env->intervals[i], (*offspring_gene)[i].binary);
			} else if (parameter == VARIABLE_TYPE_REAL){
					cross_real_and_append(father_gene[i].real, mother_gene[i].real, (*offspring_gene) + i);
					valid_part = is_valid_float(env->intervals[i], (*offspring_gene)[i].real);
			}

			if (!valid_part) {
					valid_gene = 0;
			}
		}
	} while (!valid_gene);

}

/*
* Converts int to binary representation and gets some bits from mother,
* some from father and stores them them as integer into offspring gene.
*
* parameter int      father_gene    father gene
* parameter int      mother_gene    mother gene
* parameter (gene *) offspring_gene pointer to offspring gene
*
* return void
*/
void cross_binary_and_append(int father_gene, int mother_gene, gene *offspring_gene) {
		char * offspring_gene_binary;
		char * binary_father_gene;
		char * binary_mother_gene;
		long bin_num;
		int length;
		int i;

		binary_father_gene = NULL;
		binary_mother_gene = NULL;

		/* Convert decimal to binary */
		get_binary_from_int(father_gene, &binary_father_gene);
		get_binary_from_int(mother_gene, &binary_mother_gene);
		length = strlen(binary_father_gene);

		offspring_gene_binary = malloc(sizeof(char) * length + 1);
		if (offspring_gene_binary == NULL){
				printf("Malloc failed\n");
				return;
		}

		offspring_gene_binary[length] = '\0';

		/* randomly pick bit from father or mother */
		for (i = 0; i < length; i++) {
				if (rand() % 2) {
						offspring_gene_binary[i] = binary_mother_gene[i];
				} else {
						offspring_gene_binary[i] = binary_father_gene[i];
				}
		}

		sscanf(offspring_gene_binary, "%ld", &bin_num);

		/* Convert binary to decimal */
		offspring_gene->binary = get_int_from_binary(bin_num);

		free(binary_father_gene);
		free(binary_mother_gene);
		free(offspring_gene_binary);
}

/*
* Picks given percentage of population at random and mutates them.
*
* parameter (creature *)    population          father gene
* parameter int             mutation_percentage mutation percentage
* parameter int             population_count    population count
* parameter (environment *) env                 configuration
*
* return void
*/
void mutate_population(creature *population, int mutation_percentage, int population_count, environment * env){
		creature *individual;
		int index, i;
		int count_of_mutants;
		int fittest;

		count_of_mutants = population_count / mutation_percentage;
		fittest          = 0;

		printf("\n*******************************************\n"); /* Comment tag */
		printf("********* Mutating %d creatures ************\n", count_of_mutants); /* Comment tag */
		printf("*******************************************\n\n"); /* Comment tag */

		for (i = 0; i < count_of_mutants; ) {
				index      = rand() % population_count;
				individual = get_creature_by_number(population, index);
				fittest    = individual->is_alpha;
				if (!fittest) {
						mutate_creature(individual, env);
						i++;
				}
		}
}

/*
* For given creature creates random new gene that replaces his old one.
* Only one type of gene is mutated (either REAL or BINARY, picked randomly).
*
* parameter (creature *)    individual creature to mutate
* parameter (environment *) env        configuration
*
* return void
*/
void mutate_creature(creature *individual, environment * env){
		int binary_or_real; /* like boolean */
		int i;
		int r;
		float from;
		float to;

		binary_or_real = rand() % 2;

		for (i = 0; i < env->count_of_parameters; i++) {
				sscanf(env->intervals[i], "%f,%f", &from, &to);
				r  = rand();
				r %= (int)( to - from );
				if (binary_or_real) {
						if (env->parameters[i] == VARIABLE_TYPE_INTEGER) {
								individual->gene[i].binary = (int) from + r;
						}
				} else {
						if (env->parameters[i] == VARIABLE_TYPE_REAL) {
								individual->gene[i].real  = from + (float) r;
								individual->gene[i].real += (float)rand()/(float)(RAND_MAX);
						}
				}
		}
}

/*
* Crosses real by getting average of father and mother.
*
* parameter float    father_gene    father gene as float
* parameter float    mother_gene    mother gene as float
* parameter (gene *) offspring_gene pointer to offspring gene
*
* return void
*/
void cross_real_and_append(float father_gene, float mother_gene, gene *offspring_gene) {
		offspring_gene->real = (father_gene + mother_gene) / 2;
}
