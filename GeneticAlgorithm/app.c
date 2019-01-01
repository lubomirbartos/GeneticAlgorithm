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

  //population count se muze menit, proto ukazatel
  // evolves a population for number of generations in environment
  void evolve(jedinec **population, int *population_count, int mutation_percentage, environment *env, int last_generation) {
      int i;
      jedinec *population_pointer = *population;

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
    	int population_count = 30;
      jedinec *population;
      jedinec *fittest;
      int last_generation = 0;

      create_initial_population(&population, population_count, env);

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

  // to disable info output, comment all lines containing "comment tag"
  int main(int argc, char *argv[]) {

      int count_of_generations;
      int mutation_percentage;
      char *meta_data_file;
      environment *env = malloc(sizeof(environment));
      if (env == NULL){
        printf("Malloc failed\n");
        return 0;
      }


    	handle_cmd_arguments(argc, argv, &meta_data_file, &count_of_generations, &mutation_percentage);
      get_environment(meta_data_file, &env);

    	life(count_of_generations, mutation_percentage, env);

      free(env->executable);
      free(env->meta_data_file);
      free(env->parameters);
      free(env->intervals);
      free(env);

      return 0;
  }
