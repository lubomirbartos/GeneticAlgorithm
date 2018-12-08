#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "structures.h"
#include "jedinec.h"
#include "config.h"
#include "nature.h"

void dying_time(jedinec *population, int *population_count) {
	float average_fitness;
	int i;
	get_average_fitness(population, population_count, &average_fitness);
	jedinec *pointer = population;

	while(pointer->next) {
		if(pointer->fitness < average_fitness){
			kill_creature(pointer, i);
		}
		pointer = pointer->next;
	}
}

void create_initial_population(jedinec **population, int count_of_creatures, environment *env){
    int i;
		jedinec *creature;
    *population = create_creature(env);
    jedinec *population_last = *population;
    jedinec *population_first = *population;

    for (i = 0; i < count_of_creatures - 1; i++) {
        creature = create_creature(env);
        creature->previous = population_last;
        population_last->next = creature;
        population_last = population_last->next;
    }

}

jedinec *create_creature(environment *env) {
	jedinec *creature;
	gene *gene = calloc(env->count_of_parameters, sizeof(gene));
	int fitness = 0;

	create_random_gene(gene, env);

    creature = malloc (sizeof (jedinec));
    if (creature == NULL){
        return NULL;
    }

    creature->gene = gene;
    if (!creature->gene) {
        free (creature);
        return NULL;
    }

    creature->fitness = fitness;

    return creature;
}

void create_random_gene(gene *gene, environment *env){

	int i, r, number_in_interval, from_int, to_int;
	char *from, *to, *interval_space;
	float from_real, to_real;

	char *value = malloc(100);

	srand(time(NULL));

	for (i = 0; i < env->count_of_parameters; i++) {
		r = rand();


		if (*(env->parameters + i) == VARIABLE_TYPE_INTEGER) {

			sscanf(env->intervals[i], "%d,%d", &from_int, &to_int);
			r %= (to_int - from_int);
			gene[i].binary = from_int + r;
		} else if (*(env->parameters + i) == VARIABLE_TYPE_REAL){

			sscanf(env->intervals[i], "%f,%f", &from_real, &to_real);
			number_in_interval =  r % (int)( to_real - from_real );
			gene[i].real = from_real + (float) number_in_interval;

		}
	}

	free(value);
}


void mating_time(jedinec *population, int *population_count, int mutation_percentage, environment *env) {
	int i;
	int pairs[*population_count/2][2];

	//create pairs
	for (i = 0; i < *population_count; i++) {
		pairs[i][0] = i;
		pairs[i][1] = *population_count - i;
	}

	for(i = 0; i < *population_count/2; i++) {
		breed_offspring(population, pairs[i][0], pairs[i][1], env, mutation_percentage);
	}
}

jedinec *get_creature_by_number(jedinec *population, int index) {
	int i;
	jedinec *pointer_to_creature = population;
	for (i = 0; i < index; i++) {
		pointer_to_creature = pointer_to_creature->next;
	}
	return pointer_to_creature;
}

void kill_creature(jedinec *creature, int index) {
	creature->next->previous = creature->previous;
	creature->previous->next = creature->next;
	free(creature);
}


void test_creature(jedinec * creature, float *result, environment *env) {
	char *creature_executable = env->executable;

	write_creature_metadata(creature, env);

	int BUFSIZE = 128;

    char buf[BUFSIZE];
    FILE *fp;
    char results[14][300];
    int count_of_results = 0;
    int counting_array[env->count_of_parameters];
    int i;

    if ((fp = popen(creature_executable, "r")) == NULL) {
        printf("Error opening pipe!\n");
    }

    while (fgets(results[count_of_results], BUFSIZE, fp) != NULL) {
        count_of_results++;
    }

    if(pclose(fp))  {
        printf("Command not found or exited with error status\n");
    }

    for (i = 1; i < 14 ; i++) { //from 1 because first line is "starting"
        if (i == 13) {
            *result = atof(results[i]); //this is what we want???
            continue;
        }

        //counting...48765 -> 48765
        counting_array[i] = atoi(results[i]+11);
        printf("%d.: %d \n", i, counting_array[i]);
    }
}

// pushes new offspring to population array
void breed_offspring(jedinec *population, int mother_index, int father_index, environment *env, int mutation_percentage){
	int i;
	gene *offspring_gene = calloc(env->count_of_parameters, sizeof(gene));
	jedinec *offspring;
	jedinec *last_creature = get_last_creature_in_list(population);
	gene *father_gene = get_creature_by_number(population, father_index)->gene;
	gene *mother_gene = get_creature_by_number(population, mother_index)->gene;

	cross_gene(mother_gene, father_gene, &offspring_gene, env, mutation_percentage);

	offspring = malloc(sizeof(jedinec));
	offspring->gene = offspring_gene;

	last_creature->next = offspring;
	offspring->previous = last_creature;
}

jedinec *get_last_creature_in_list(jedinec *population) {
	jedinec *pointer = population;
	while (pointer->next != NULL) {
		pointer = pointer->next;
	}
	return pointer;
}


void cross_gene(gene *mother_gene, gene *father_gene, gene **offspring_gene, environment *env, int mutation_percentage){
	int i, f_bin_gene, m_bin_gene;
	float f_real_gene, m_real_gene;
	char parameter;

	for (i = 0; i < env->count_of_parameters; i++) {
		parameter = env->parameters[i]; // env part of gene

		if (parameter == VARIABLE_TYPE_INTEGER) {
			f_bin_gene = father_gene[i].binary;
			m_bin_gene = mother_gene[i].binary;
 			cross_binary_and_append(f_bin_gene, m_bin_gene, (*offspring_gene) + i, mutation_percentage);
		} else if (parameter == VARIABLE_TYPE_REAL){
			f_real_gene = father_gene[i].real;
			m_real_gene = mother_gene[i].real;
 			cross_real_and_append(f_real_gene, m_real_gene, (*offspring_gene) + i, mutation_percentage);
		}
	}
}

void cross_binary_and_append(int father_gene, int mother_gene, gene *offspring_gene_int, int mutation_percentage) {
	int i;
	long long bin_num;
	char * binary_father_gene = get_binary_from_int(father_gene);
	char * binary_mother_gene = get_binary_from_int(mother_gene);
	char *offspring_gene_binary = malloc(sizeof(char) * strlen(binary_father_gene));
	int length = strlen(binary_father_gene)-1;
	int half = length/2;
	for (i = 0; i < half; i++) {
		offspring_gene_binary[i] = binary_father_gene[i];
		offspring_gene_binary[length - i] = binary_father_gene[length - i];
	}

	sscanf(offspring_gene_binary, "%lld", &bin_num);

	offspring_gene_int->binary = get_int_from_binary(bin_num);
}

void cross_real_and_append(float father_gene, float mother_gene, gene *offspring_gene_real, int mutation_percentage) {
	offspring_gene_real->real = (father_gene + mother_gene) / 2;
}

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
