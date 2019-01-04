  #include <stdio.h>
  #include <assert.h>
  #include <string.h>
  #include <stdlib.h>
  #include "../lib/structures.h"
  #include "../lib/config.h"
  #include "../lib/nature.h"

  // to disable info output, comment all lines
  // in config.c and nature.case containing "comment tag"
  int main(int argc, char *argv[]) {

      int i;
      int count_of_generations;
      int mutation_percentage;
      char *meta_data_file;
      environment *env = malloc(sizeof(environment));
      if (env == NULL){
        printf("Malloc failed\n");
        return 0;
      }

      if (argc < 3) {
          printf("Error: Expected at least 2 arguments! Actual count of arguments: %d", argc-1); // prvni argument je soubor spusteni aplikace, proto (argc-1)
          return 0;
      }

      // handle arguments
      if (!argv[1] || !argv[2]) {
        return 0;
      }


      meta_data_file = malloc((strlen(argv[1]) + 1) * sizeof(char));
      if (meta_data_file == NULL){
        printf("Malloc failed\n");
        return 0;
      }

      meta_data_file[strlen(argv[1])] = '\0';
      strcpy(meta_data_file, argv[1]);


      // count_of_generations = atoi(argv[2]);
      sscanf(argv[2], "%d", &count_of_generations);



      if ((argc == 5) && argv[3] && (argv[3][0] == '-') && (argv[3][1] == 'm') && argv[4]) {

          // mutation_percentage = atoi(argv[4]);
          sscanf(argv[4], "%d", &mutation_percentage);

          // printf("Malloc failed %s\n", argv[3]);

      } else {
          mutation_percentage = DEFAULT_MUTATION_RATE; //default 5%
      }

      if (meta_data_file) {
        get_environment(meta_data_file, &env);
      }


    	life(count_of_generations, mutation_percentage, env);

      free(env->executable);
      free(env->meta_data_file);
      free(env->parameters);
      for (i = 0; i < env->count_of_parameters; i++) {
        free(env->intervals[i]);
      }
      free(env->variable_names);
      free(env->intervals);
      free(env);
      free(meta_data_file);

      return 0;
  }
