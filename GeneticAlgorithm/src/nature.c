#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <float.h>
#include "../lib/structures.h"
#include "../lib/config.h"
#include "../lib/nature.h"

//population count se muze menit, proto ukazatel
// evolves a population for number of generations in environment
void evolve(creature **population, int *population_count, int mutation_percentage, environment *env, int last_generation) {
		int i;
		creature *population_pointer = *population;

		printf("\n*******************************************\n"); //Comment tag
		printf("****** GETTING FITNESS FOR CREATURES ******\n"); //Comment tag
		printf("*******************************************\n\n"); //Comment tag

		for(i = 0; i < *population_count; i++) {
				test_creature(population_pointer, env);
				population_pointer = population_pointer->next;
		}

		dying_time(population, population_count); //die before you fuck, weakling!

		assert(*population_count != 0);
		if (!last_generation) {
			mating_time(population, population_count, env);
			mutate_population(*population, mutation_percentage, *population_count, env);
		}
}

//starts the evolution with given arguments
// int count_of_generations - count of generations
// int mutation_percentage - mutation percentage
void life(int count_of_generations, int mutation_percentage, environment *env) {
		int generation_number;
		int population_count = 0;
		creature *population;
		creature *fittest;
		int last_generation = 0;

		create_initial_population(&population, &population_count, env);

	for ( generation_number = 0 ; generation_number < count_of_generations ; generation_number++ ) {
		if (generation_number == (count_of_generations - 1)) {
			last_generation = 1;
		}
		printf("\n\n ..........Starting generation %d..........\n", generation_number); //Comment tag

		evolve(&population, &population_count, mutation_percentage, env, last_generation);
		remove_alpha_tags(population);

		log_fittest(population, generation_number, env);
		printf("\n\n ...........Ending generation %d...........\n\n", generation_number); //Comment tag

	}

	kill_all(population);
}


// Kills creatures that have below average fitness
void dying_time(creature **population, int *population_count) {
	float average_fitness;
	int i;
	get_average_fitness(*population, *population_count, &average_fitness);
	creature *pointer = *population;
	creature *weakling;
	printf("\n*******************************************\n"); //Comment tag
	printf("*************** DYING TIME ****************\n"); //Comment tag
	printf("*******************************************\n\n"); //Comment tag


	printf("..Average fitness of population      \t%f \n", average_fitness); //Comment tag
	printf("..Population count before the purge  \t%d \n\n", *population_count); //Comment tag

	while(pointer) {
		if(pointer->fitness < average_fitness && *population_count > 10 ){

			if (!pointer->previous) { //first member, we need to change population pointer
				*population = pointer->next; // TODO but what if there is only one and no next?
			}
			weakling = pointer;
			pointer = pointer->next;
			kill_creature(weakling);
			(*population_count)--;
			weakling = NULL;
		} else {
			pointer = pointer->next;
		}
	}
	printf("\n..Population count after the purge:  \t%d \n", *population_count); //Comment tag

}

// iterates over population and counts the average population
void get_average_fitness(creature *population, int population_count, float *average_fitness){
	float sum = 0;
	creature *pointer = population;

	// sum of all fitnesses
	for(int i = 0; i < population_count; i++) {
		sum += pointer->fitness;
		pointer = pointer->next;
	}

	//save average
	*average_fitness = sum/(population_count);

}


// Creates initial population with given count and according to configuration
void create_initial_population(creature **population, int *count_of_creatures, environment *env){
	int i;
	creature *creation;
	creature *last;
	*population = create_creature(env);
	(*population)->previous = NULL;
	(*population)->first = 1;
	(*population)->last = 1;
	last = *population;

	for (i = 1; i < POPULATION_LIMIT; i++) {
		creation = create_creature(env);
		sprintf(creation->name, "%d", i);
		creation->previous = last;
		last->last = 0;
		creation->last = 1;
		creation->first = 0;
		last->next = creation;
		last = creation;
		(*count_of_creatures)++;
	}

}

// Creates creature with random gene according to configuration
creature *create_creature(environment *env) {
	creature *creation;
	gene *gene = calloc(env->count_of_parameters, sizeof(gene));

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

	if (creation == NULL){
		return NULL;
	}

	creation->gene = gene;
	if (!creation->gene) {
		free (creation);
		free (gene);
		return NULL;
	}
	// printf("Malloc failed %d\n", creature->previous);

	creation->fitness = 0;
	creation->is_alpha = 0;

	return creation;
}

/////////////////////////////////////////////////////////
// Creates random gene according to passed configuration
// and stores it to first argument
//
// param  (gene*)         gene  storage for gene
// param  (environment *) env   configuration
//
// void
////////////////////////////////////////////////////////
void create_random_gene(gene *gene, environment *env){
	int i, r;
	float from, to;

	for (i = 0; i < env->count_of_parameters; i++) {
		r = rand();
		sscanf(env->intervals[i], "%f,%f", &from, &to); //TODO valgrind invalid read

		r %= (int)( to - from );
		if (*(env->parameters + i) == VARIABLE_TYPE_INTEGER) {
			gene[i].binary = (int)from + r;
		} else if (*(env->parameters + i) == VARIABLE_TYPE_REAL){
			gene[i].real = from + (float) r;
		}
	}
}

int get_valuable_creature_index(creature *population, int last_creature_index) {
	float average_fitness;
	creature *valuable_creature;
	int index;
	float fitness = FLT_MIN;

	get_average_fitness(population, last_creature_index, &average_fitness);

	while (fitness < average_fitness) {
		index = rand() % last_creature_index;
		valuable_creature = get_creature_by_number(population, index);
		fitness = valuable_creature->fitness;
	}
	return index;
}

// Creates random pairs that breed new members of population
// Children are appended to end of list
void mating_time(creature **population, int *population_count, environment *env) {
	int mother_index = 0, father_index = 0;
	int last_creature_index;
	printf("\n*******************************************\n"); //Comment tag
	printf("************** MATING TIME ****************\n"); //Comment tag
	printf("*******************************************\n\n"); //Comment tag
	// print_population(*population);

	last_creature_index = *population_count -1; //from 0 until last adult
	while (*population_count < POPULATION_LIMIT) {
		// printf("Current population count:     %d \n", *population_count);

		while (mother_index == father_index) {
			//strong father and random mother, because if we pick
			//only creatures with high fittness, the weaker ones will never breed
			father_index = get_valuable_creature_index(*population, last_creature_index);
			mother_index = rand() % last_creature_index;
		}
		// printf("Father:     %d \n", father_index);
		// printf("Mother:     %d \n", mother_index);

		breed_offspring(*population, father_index, mother_index, env);

		mother_index = 0;
		father_index = 0;

		(*population_count)++;
	}

}

// Iterates over population until given index and returns pointer to creature
creature *get_creature_by_number(creature *population, int index) {
	int i;
	creature *pointer_to_creature = population;
	// print_population(population);

	for (i = 0; i < index; i++) {
		// printf("<UwU>   %f\n", pointer_to_creature->gene[0].real);

		pointer_to_creature = pointer_to_creature->next;
	}

	return pointer_to_creature;
}


void remove_alpha_tags(creature *population) {
	int i;
	creature *pointer = population;

	do  {
		pointer->is_alpha = 0;
	} while (pointer = pointer->next);
}

// Makes adjecent creatures point to each other and kills (frees) creature
// memory clean after creature
void kill_creature(creature *individual) {
	if (individual->first && individual->last) {
		printf("!Killing last creature!\n"); //Comment tag
	} else if (individual->first) {
		individual->next->previous = NULL;
		individual->next->first = 1;
	} else if (individual->last) {
		individual->previous->next = NULL;
		individual->previous->last = 1;
	} else {
		individual->previous->next = individual->next;
		individual->next->previous = individual->previous;
	}

	printf("..Creature with fitness\t %f \tkilled!\n", individual->fitness); //Comment tag

	free(individual->gene);
	free(individual);
}

// Writes creature data to meta data file,
// executes executable from command line and gets result,
// which will be stored as fitness of creature
void test_creature(creature * individual, environment *env) {
	int BUFSIZE = 128;
	FILE *fp;
	char result[100];
	int count_of_results = 0;
	char path_buf[PATH_MAX + 100];
	// printf("LOL %s     !\n", env->executable);
	// printf("LOL %s!\n", env->executable);
	// printf("LOL %s!\n", env->executable);
	// printf("LOL %s!\n", env->executable);
// exit(0);
// printf("LOL %s!\n", env->executable);
// exit(0);

	char * path = realpath(env->executable, path_buf); //TODO replace with env->executable

	write_creature_metadata(individual, env);

	if ((fp = popen(path_buf, "r")) == NULL) {  //TODO not multiplatform
		printf("Error opening pipe!\n");
	}

	while (fgets(result, BUFSIZE, fp) != NULL) {
		count_of_results++;
	}

	if(pclose(fp))  {
		printf("Command not found or exited with error status\n");
	}

	individual->fitness = atof(result);
	log_results(individual, env);
	printf("..Creature got fitness %f \n", individual->fitness); //Comment tag

}

// Creates new offspring with gene based on its parents' genes.
// Pushes new offspring to population list.
void breed_offspring(creature *population, int mother_index, int father_index, environment *env){

	gene *offspring_gene = malloc(env->count_of_parameters * sizeof(gene));
	if (!offspring_gene) {
		printf("Malloc failed\n");
		return;
	}

	creature *last_creature = get_last_creature_in_list(population);

	gene *father_gene = get_creature_by_number(population, father_index)->gene;
	gene *mother_gene = get_creature_by_number(population, mother_index)->gene;

	cross_gene(mother_gene, father_gene, &offspring_gene, env);

	last_creature->next = (creature*) malloc(sizeof(creature));
	if (last_creature->next == NULL){
		printf("Malloc failed\n");
		return;
	}

	last_creature->next->previous = last_creature;
	last_creature->next->next = NULL;

	sprintf(last_creature->next->name, "%d%d", mother_index, father_index);
	last_creature->next->fitness = FLT_MIN;
	last_creature->next->is_alpha = 0;
	last_creature->last = 0;
	last_creature->next->last = 1;
	last_creature->next->first = 0;
	last_creature->next->gene = (gene*) calloc(env->count_of_parameters, sizeof(gene));
	if (last_creature->next->gene == NULL){
		printf("Malloc failed\n");
		return;
	}

	copy_gene(last_creature->next->gene, offspring_gene, env);

	printf("..Parents \t%d and %d \tcreated offspring!\n", father_index, mother_index); //Comment tag

	free(offspring_gene);
	last_creature = NULL;
}

// Copies data according to congiration from one gene to another
void copy_gene(gene *to, gene *from, environment *env){
	int i;
	for (i = 0; i < env->count_of_parameters; i++) {
		if (env->parameters[i] == VARIABLE_TYPE_INTEGER) {
			to[i].binary = from[i].binary;
		} else if (env->parameters[i] == VARIABLE_TYPE_REAL) {
			to[i].real = from[i].real;
		}
	}
}

// prints info about all members of population
void print_population(creature *population){
	creature *pointer = population;
	int count = 0;
	printf("Printing population \n"); //Comment tag

		do  {
			printf("\nCreature \t  %d \n", count); //Comment tag
			printf("\t\t name %s \n", pointer->name); //Comment tag
			printf("\t\t fitness %f \n", pointer->fitness); //Comment tag
			printf("\t\t gene real %f \n", pointer->gene[0].real); //Comment tag
			printf("\t\t gene binary %d \n", pointer->gene[1].binary); //Comment tag
			count++;
		} while (pointer = pointer->next);
}

void *log_results(creature *individual, environment *env){
	char log[100];
	int i;

	FILE * file_pointer;
	file_pointer = fopen("../val.txt", "a");
	if (file_pointer == NULL)
	{
		/* Unable to open file hence exit */
		printf("\nUnable to open file.\n");
		exit(0);
	}

	sprintf(log, "%f\n", individual->fitness);
	printf("\%s.\n", log);
	// exit(0);

	fputs(log, file_pointer);
	memset(log, 0, sizeof log);

	for (i = 0; i < env->count_of_parameters; i++) {
		if (env->parameters[i] == VARIABLE_TYPE_INTEGER) {
			sprintf(log, "%c=%d#(%s);Z\n", env->variable_names[i], individual->gene[i].binary, env->intervals[i]);
		} else {
			sprintf(log, "%c=%f#(%s);R\n", env->variable_names[i], individual->gene[i].real, env->intervals[i]);
		}

		fputs(log, file_pointer);
		memset(log, 0, sizeof log);
	}

	fputs("\n", file_pointer);

	fclose(file_pointer);
}

void *log_fittest(creature *population,int generation_number, environment *env){
		creature *pointer = population;
		creature *fittest = population;
		char log[100];
		int i;

		FILE * file_pointer;
		file_pointer = fopen("../gen.txt", "a");
		if (file_pointer == NULL)
		{
			/* Unable to open file hence exit */
			printf("\nUnable to open file.\n");
			exit(0);
		}

		do  {
				if (pointer->fitness > fittest->fitness) {
						fittest = pointer;
				}
		} while (pointer = pointer->next);
		fittest;
		fittest->is_alpha = 1;

		printf("..Alpha creature has fitness:\t\t%f  \n\n", fittest->fitness); //Comment tag

		sprintf(log, "--- GENERATION %d ---\n", generation_number);
		fputs(log, file_pointer);
		memset(log, 0, sizeof log);

		sprintf(log, "%f\n", fittest->fitness);
		fputs(log, file_pointer);
		memset(log, 0, sizeof log);

		for (i = 0; i < env->count_of_parameters; i++) {
			if (env->parameters[i] == VARIABLE_TYPE_INTEGER) {
				sprintf(log, "%c=%d#(%s);Z\n", (char) (65+i), fittest->gene[i].binary, env->intervals[i]);
				fputs(log, file_pointer);
				memset(log, 0, sizeof log);
			} else {
				sprintf(log, "%c=%f#(%s);R\n", (char) (65+i), fittest->gene[i].real, env->intervals[i]);
				fputs(log, file_pointer);
				memset(log, 0, sizeof log);
			}

		}
		fputs("\n", file_pointer);

		fclose(file_pointer);

}


// returns pointer to last creature in population
creature *get_last_creature_in_list(creature *population) {
	creature *pointer = population;

	while (pointer->next) {
		pointer = pointer->next;
	}

	return pointer;
}

// Kills whole population
void kill_all(creature *population) {
	creature *pointer = population;
	creature *individual;
	int some_alive = 1;

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

// Crosses father and mother gene according to configuration an mutates
void cross_gene(gene *mother_gene, gene *father_gene, gene **offspring_gene, environment *env){
	int i = 0, f_bin_gene, m_bin_gene;
	float f_real_gene, m_real_gene;
	char parameter;
	int valid_gene;
	int valid_part;

	do  {

		valid_gene = 1;

		for (i = 0; i < env->count_of_parameters; i++) {
			parameter = env->parameters[i]; // env part of gene
			if (parameter == VARIABLE_TYPE_INTEGER) {
				f_bin_gene = father_gene[i].binary;
				m_bin_gene = mother_gene[i].binary;
				cross_binary_and_append(f_bin_gene, m_bin_gene, (*offspring_gene) + i);
				valid_part = is_valid_int(env->intervals[i], (*offspring_gene)[i].binary);
			} else if (parameter == VARIABLE_TYPE_REAL){
				f_real_gene = father_gene[i].real;
				m_real_gene = mother_gene[i].real;
				cross_real_and_append(f_real_gene, m_real_gene, (*offspring_gene) + i);
				valid_part = is_valid_float(env->intervals[i], (*offspring_gene)[i].real);
			}

			if (!valid_part) {
				valid_gene = 0;
			}
		}

	} while (!valid_gene);

}

// Converts int to binary representation and gets some bits from mother,
// some from father and stores them them as integer into offspring gene
void cross_binary_and_append(int father_gene, int mother_gene, gene *offspring_gene) {
	int i;
	long long bin_num;

	char * binary_father_gene = NULL;
	char * binary_mother_gene = NULL;
	int mutated_gene;
	get_binary_from_int(father_gene, &binary_father_gene);
	get_binary_from_int(mother_gene, &binary_mother_gene);

	int length = strlen(binary_father_gene);
	char *offspring_gene_binary = malloc(sizeof(char) * length + 1);
	if (offspring_gene_binary == NULL){
		printf("Malloc failed\n");
		return;
	}

	offspring_gene_binary[length] = '\0';

	// randomly pick from father/mother
	for (i = 0; i < length; i++) {
		if (rand() % 2) {
			offspring_gene_binary[i] = binary_mother_gene[i];
		} else {
			offspring_gene_binary[i] = binary_father_gene[i];
		}
	}
	sscanf(offspring_gene_binary, "%lld", &bin_num);

	offspring_gene->binary = get_int_from_binary(bin_num);


	free(binary_father_gene);
	free(binary_mother_gene);
	free(offspring_gene_binary);
}

void mutate_population(creature *population, int mutation_percentage, int population_count, environment * env){
	int index, i;
	creature *individual;
	int count_of_mutants = population_count / mutation_percentage;
	int unkillable = 0;
	// printf("\n*******************************************\n"); // Comment tag
	// printf("********* Mutating %d creatures ************\n", count_of_mutants); // Comment tag
	// printf("*******************************************\n\n"); // Comment tag
	for (i = 0; i < count_of_mutants; ) {
		index = rand() % population_count;
		individual = get_creature_by_number(population, index);
		unkillable = individual->is_alpha;
		if (!unkillable) {
			mutate_creature(individual, env);
			i++;
		}
	}
}

void mutate_creature(creature *individual, environment * env){
	int i;
	int r;
	int binary_or_real = rand() % 2;
	float from, to;


	for (i = 0; i < env->count_of_parameters; i++) {
		r = rand();
		sscanf(env->intervals[i], "%f,%f", &from, &to);
		r %= (int)( to - from );
		if (binary_or_real) {
			if (env->parameters[i] == VARIABLE_TYPE_INTEGER) {
				individual->gene[i].binary = (int)from + r;
			}
		} else {
			if (env->parameters[i] == VARIABLE_TYPE_REAL) {
				individual->gene[i].real = from + (float) r;
			}
		}
	}
}



// Crosses real by getting average of father and mother
void cross_real_and_append(float father_gene, float mother_gene, gene *offspring_gene) {


	offspring_gene->real = (father_gene + mother_gene) / 2;


}
