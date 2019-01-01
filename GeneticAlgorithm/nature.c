#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include "structures.h"
#include "jedinec.h"
#include "config.h"
#include "nature.h"


// Kills creatures that have below average fitness
void dying_time(jedinec **population, int *population_count) {
	float average_fitness;
	int i;
	get_average_fitness(*population, population_count, &average_fitness);
	jedinec *pointer = *population;
	jedinec *weakling;
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

// Creates initial population with given count and according to configuration
void create_initial_population(jedinec **population, int count_of_creatures, environment *env){
	int i;
	jedinec *creature;
	jedinec *population_last;
	*population = create_creature(env);
	population_last = *population;

	for (i = 1; i < count_of_creatures; i++) {
		creature = create_creature(env);
		sprintf(creature->name, "%d", i);
		creature->previous = population_last;
		population_last->next = creature;
		population_last = creature;
	}

}

// Creates creature with random gene according to configuration
jedinec *create_creature(environment *env) {
	jedinec *creature;
	gene *gene = calloc(env->count_of_parameters, sizeof(gene));
	if (gene == NULL){
		printf("Malloc failed\n");
		return NULL;
	}

	create_random_gene(gene, env);
	creature = calloc (1, sizeof (jedinec));
	if (creature == NULL){
		printf("Malloc failed\n");
		return NULL;
	}

	if (creature == NULL){
		return NULL;
	}

	creature->gene = gene;
	if (!creature->gene) {
		free (creature);
		free (gene);
		return NULL;
	}

	creature->fitness = 0;
	creature->is_alpha = 0;

	return creature;
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

// Creates random pairs that breed new members of population
// Children are appended to end of list
void mating_time(jedinec **population, int *population_count, environment *env) {
	int random_mother = 0, random_father = 0;
	int last_creature_index;
	printf("\n*******************************************\n"); //Comment tag
	printf("************** MATING TIME ****************\n"); //Comment tag
	printf("*******************************************\n\n"); //Comment tag
	// print_population(*population);

	last_creature_index = *population_count -1; //from 0
	while (*population_count < 30) {
		// printf("Current population count:     %d \n", *population_count);

		while (random_mother == random_father) {
			random_father = rand();
			random_mother = rand();
			random_father %= last_creature_index;
			random_mother %= last_creature_index;
		}
		// printf("Father:     %d \n", random_father);
		// printf("Mother:     %d \n", random_mother);

		breed_offspring(*population, random_father, random_mother, env);

		random_mother = 0;
		random_father = 0;

		(*population_count)++;
	}

}

// Iterates over population until given index and returns pointer to creature
jedinec *get_creature_by_number(jedinec *population, int index) {
	int i;
	jedinec *pointer_to_creature = population;
	// print_population(population);

	for (i = 0; i < index; i++) {
		// printf("<UwU>   %f\n", pointer_to_creature->gene[0].real);

		pointer_to_creature = pointer_to_creature->next;
	}

	return pointer_to_creature;
}


void remove_alpha_tags(jedinec *population) {
	int i;
	jedinec *pointer = population;

	do  {
		pointer->is_alpha = 0;
	} while (pointer = pointer->next);
}

// Makes adjecent creatures point to each other and kills (frees) creature
// memory clean after creature
void kill_creature(jedinec *creature) {
	int first = 0, last = 0;

	if (!creature->next) {
		last = 1;
	}

	if (!creature->previous) {
		first = 1;
	}

	if (first && last) {
		printf("!Killing last creature!\n"); //Comment tag
	} else if (first) {
		creature->next->previous = NULL;
	} else if (last) {
		creature->previous->next = NULL;
	} else {
		creature->previous->next = creature->next;
		creature->next->previous = creature->previous;
	}

	printf("..Creature with fitness\t %f \tkilled!\n", creature->fitness); //Comment tag

	free(creature->gene);
	free(creature);
}

// Writes creature data to meta data file,
// executes executable from command line and gets result,
// which will be stored as fitness of creature
void test_creature(jedinec * creature, environment *env) {
	int BUFSIZE = 128;
	FILE *fp;
	char result[100];
	int count_of_results = 0;
	char path_buf[PATH_MAX + 100];

	char * path = realpath("func", path_buf); //TODO replace with env->executable

	write_creature_metadata(creature, env);

	if ((fp = popen(path_buf, "r")) == NULL) {  //TODO not multiplatform
		printf("Error opening pipe!\n");
	}

	while (fgets(result, BUFSIZE, fp) != NULL) {
		count_of_results++;
	}

	if(pclose(fp))  {
		printf("Command not found or exited with error status\n");
	}

	creature->fitness = atof(result);
	printf("..Creature got fitness %f \n", creature->fitness); //Comment tag

}

// Creates new offspring with gene based on its parents' genes.
// Pushes new offspring to population list.
void breed_offspring(jedinec *population, int mother_index, int father_index, environment *env){

	gene *offspring_gene = malloc(env->count_of_parameters * sizeof(gene));
	if (!offspring_gene) {
		printf("Malloc failed\n");
		return;
	}

	jedinec *last_creature = get_last_creature_in_list(population);

	gene *father_gene = get_creature_by_number(population, father_index)->gene;
	gene *mother_gene = get_creature_by_number(population, mother_index)->gene;

	cross_gene(mother_gene, father_gene, &offspring_gene, env);

	last_creature->next = (jedinec*) malloc(sizeof(jedinec));
	if (last_creature->next == NULL){
		printf("Malloc failed\n");
		return;
	}

	last_creature->next->previous = last_creature;
	last_creature->next->next = NULL;

	sprintf(last_creature->next->name, "%d%d", mother_index, father_index);
	last_creature->next->fitness = FLT_MIN;
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
void print_population(jedinec *population){
	jedinec *pointer = population;
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

void *log_fittest(jedinec *population,int generation_number, environment *env){
		jedinec *pointer = population;
		jedinec *fittest = population;
		char log[100];
		int i;

		FILE * file_pointer;
		file_pointer = fopen("fittest_creatures.txt", "a");
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

		sprintf(log, "\n\n--- GENERATION %d ---\n", generation_number);
		fputs(log, file_pointer);
		memset(log, 0, sizeof log);

		sprintf(log, "Fitness: \t %f\n\n", fittest->fitness);
		fputs(log, file_pointer);
		memset(log, 0, sizeof log);

		for (i = 0; i < env->count_of_parameters; i++) {
			if (env->parameters[i] == VARIABLE_TYPE_INTEGER) {
				sprintf(log, "%c \t\t = \t %d \t\t\t\t #(%s);Z\n", (char) (65+i), fittest->gene[i].binary, env->intervals[i]);
				fputs(log, file_pointer);
				memset(log, 0, sizeof log);
			} else {
				sprintf(log, "%c \t\t = \t %f \t #(%s);Z\n", (char) (65+i), fittest->gene[i].real, env->intervals[i]);
				fputs(log, file_pointer);
				memset(log, 0, sizeof log);
			}

		}
		fclose(file_pointer);

}


// returns pointer to last creature in population
jedinec *get_last_creature_in_list(jedinec *population) {
	jedinec *pointer = population;

	while (pointer->next) {
		pointer = pointer->next;
	}

	return pointer;
}

// Kills whole population
void kill_all(jedinec *population) {
	jedinec *pointer = population;
	jedinec *creature;
	int some_alive = 1;

	while (some_alive) {
		creature = pointer;
		if (pointer->next) {
			pointer = pointer->next;
		} else {
			some_alive = 0;
		}

		kill_creature(creature);
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

	offspring_gene_binary[length+1] = '\0';

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

void mutate_population(jedinec *population, int mutation_percentage, int population_count, environment * env){
	int index, i;
	jedinec *creature;
	int count_of_mutants = population_count / mutation_percentage;
	printf("\n*******************************************\n");
	printf("********* Mutating %d creatures ************\n", count_of_mutants);
	printf("*******************************************\n\n");
	for (i = 0; i < count_of_mutants; ) {
		index = rand() % population_count;
		creature = get_creature_by_number(population, index);
		if (!creature->is_alpha) {
			mutate_creature(creature, env);
			i++;
		}
	}
}

void mutate_creature(jedinec *creature, environment * env){
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
				creature->gene[i].binary = (int)from + r;
			}
		} else {
			if (env->parameters[i] == VARIABLE_TYPE_REAL) {
				creature->gene[i].real = from + (float) r;
			}
		}
	}
}



// Crosses real by getting average of father and mother
void cross_real_and_append(float father_gene, float mother_gene, gene *offspring_gene) {


	offspring_gene->real = (father_gene + mother_gene) / 2;


}

// returns integer from long long number
// of ones and zeros representing binary number
int get_int_from_binary(long long n){

	int decimalNumber = 0, i = 0, remainder;
	while (n!=0)
	{
		remainder = n%10;
		n /= 10;
		decimalNumber += remainder * pow(2,i);
		++i;
	}
	return decimalNumber;

}
