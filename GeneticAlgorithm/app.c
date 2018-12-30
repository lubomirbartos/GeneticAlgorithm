#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "jedinec.h"
#include "config.h"
#include "cmd_helper.h"
#include "nature.h"
#include "app.h"

double evaluate(char *result) {
    for (int i = 0; i < 14; i++) {
    }
    return 1.0;
}


void write_to_file() {
    printf("writing to file\n");
}

//population count se muze menit, proto ukazatel
// evolves a population for number of generations in environment
void evolve(jedinec **population, int *population_count, int mutation_percentage, environment *env, int last_generation) {
    int i;
    jedinec *population_pointer = *population;

    for(i = 0; i < *population_count; i++) {

        test_creature(population_pointer, env);

        population_pointer = population_pointer->next;

    }


    dying_time(population, population_count); //die before you fuck, weakling!

    assert(*population_count != 0);
    if (!last_generation) {
      mating_time(population, population_count, mutation_percentage, env);
    }


}

//starts the evolution with given arguments
// int count_of_generations - count of generations
// int mutation_percentage - mutation percentage
void life(int count_of_generations, int mutation_percentage, environment *env) {
  	int generation_number;
  	int population_count = 30;
    jedinec *population;
    int last_generation = 0;

    create_initial_population(&population, population_count, env);

	for ( generation_number = 0 ; generation_number < count_of_generations ; generation_number++ ) {
    if (generation_number == (count_of_generations - 1)) {
      last_generation = 1;
    }

		evolve(&population, &population_count, mutation_percentage, env, last_generation);
	}

  kill_all(population);
}

int main(int argc, char *argv[]) {

    int count_of_generations;
    int mutation_percentage;
    char *meta_data_file;
    environment *env;


  	handle_cmd_arguments(argc, argv, &meta_data_file, &count_of_generations, &mutation_percentage);
    get_environment(meta_data_file, &env);

  	life(count_of_generations, mutation_percentage, env);

    printf(" \t\t\t\t\t\t\t\t creature %d \n", mutation_percentage);
    // free(meta_data_file);
    free(env->executable);
    free(env->meta_data_file);
    free(env->parameters);
    free(env->intervals);
    free(env);

    return 0;
}
