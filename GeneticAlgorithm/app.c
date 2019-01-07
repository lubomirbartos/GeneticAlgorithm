#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "config.h"
#include "nature.h"

/*
* Main of this program.
* Handles arguments from command line,
* loads configuration,
* executes life.
*
* parameter int      population count of arguments from command line
* parameter (char *) argv[]     pointer arguments from command line
*
* returns void
*/
/* to disable info output, comment all lines */
/* in config.c and nature.case containing "comment tag" */
int main(int argc, char *argv[]) {
  int count_of_generations;
  int mutation_percentage;
  int i;
  char *meta_data_file;
  environment *env;

  env = malloc(sizeof(environment));
  if (env == NULL){
    printf("Malloc of env failed\n");
    return 0;
  }

  if (argc < 3) {
    /* first argument is our executable, hence argc-1 */
    printf("Error: Expected at least 2 arguments! Actual count of arguments: %d", argc-1);
    return 0;
  }


  /* handle arguments */
  if (!argv[1] || !argv[2] || !argv[3]) {
    printf("Did you forgot some arguments?\n");
    return 0;
  }


  meta_data_file = malloc((strlen(argv[1]) + 1) * sizeof(char));
  if (meta_data_file == NULL){
    printf("Malloc failed\n");
    return 0;
  }

  meta_data_file[strlen(argv[1])] = '\0';
  strcpy(meta_data_file, argv[1]);

  sscanf(argv[2], "%d", &count_of_generations);

  if ((argc == 5) && argv[3] && (argv[3][0] == '-') && (argv[3][1] == 'm') && argv[4]) {
    sscanf(argv[4], "%d", &mutation_percentage);
    if (!is_valid_int("0, 100", mutation_percentage)){
      printf("Mutation percentage you entered is invalid!\n");
      exit(0);
    }
  } else {
    mutation_percentage = DEFAULT_MUTATION_RATE; /* default 5% */
  }

  if (meta_data_file) {
    get_environment(meta_data_file, &env);
  }

  life(count_of_generations, mutation_percentage, env);

  for (i = 0; i < env->count_of_parameters; i++) {
    free(env->intervals[i]);
  }
  free(env->intervals);
  free(env->variable_names);
  free(env->executable);
  free(env->meta_data_file);
  free(env->parameters);
  free(env);
  free(meta_data_file);

  return 0;
}
