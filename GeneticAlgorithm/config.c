#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <math.h>
#include "structures.h"
#include "config.h"
#include "nature.h"

/* reads file with meta data and stores configuration to env */
/* env->executable */
/*      meta_data_file */
/*      count_of_parameters */
/*      parameters */
/*      intervals */
void get_environment(char* file_name, environment **env) {
  char ** new_intervals;
  char ** intervals;
  char *  variable_names;
  char *  executable;
  char *  parameters;
  char *  interval;
  char line[BUFSIZE];
  char type[2];

  FILE * file_pointer;

  int param_count;
  int interval_size;
  int variable_flag;

  variable_names = malloc(sizeof(char));
  if (variable_names == NULL){
    printf("Malloc failed\n");
    return;
  }

  parameters = malloc(sizeof(char));
  if (parameters == NULL){
    printf("Malloc failed\n");
    return;
  }

  parameters[0]     = '\0';
  variable_names[0] = '\0';
  interval_size     = 50;
  param_count       = 0;
  variable_flag     = 0;
  intervals         = NULL;
  file_pointer      = fopen(file_name, "r");

  if (file_pointer == NULL) {
    printf("File pointer is NULL!\n");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL)); /* create seed for random number generator*/

  /**********************************************************/
  /*   read configuration from metadata file line by line   */
  /**********************************************************/

  while ((fgets(line, BUFSIZE, file_pointer)) != NULL) {

    if (line[1] == '\n') {
      continue;
    }

    if (intervals) {
      new_intervals = realloc(intervals, sizeof(*intervals) * (param_count+1));
      if (new_intervals){
        intervals = new_intervals;
      }else {
        printf("Realloc failed\n");
      }
    }else {
      new_intervals = (char**)calloc(param_count, sizeof(char *));
      if (!new_intervals){
        printf("Malloc failed\n");
      }
      intervals = new_intervals;

    }

    if (line[0] == '#') {

      if (strstr(line,  "#_(") != NULL) {
        /**********************************************************/
        /*            Line with details of variable               */
        /**********************************************************/
        interval = malloc(interval_size * sizeof(char));
        store_variable_from_line(line, interval, type);
        type[1] = '\0';
        parameters = (char *) realloc(parameters, (param_count + 2) * sizeof(char));
        if (parameters == NULL){
          printf("Malloc failed\n");
          return;
        }
        parameters[param_count] = type[0];
        parameters[param_count+1] = '\0';

        intervals[param_count] = malloc(interval_size * sizeof(char));
        intervals[param_count][0] = '\0';
        strcpy(intervals[param_count], interval);
        free(interval);
        param_count++;
        variable_flag = 1;
      } else {
        /**********************************************************/
        /*              Line with executable file                 */
        /**********************************************************/
        executable = malloc(sizeof(char) * strlen(line) + 1);
        if (executable == NULL){
          printf("Malloc failed\n");
          return;
        }

        sscanf(line, "#_%s", executable);

      }
    } else {
      if (variable_flag) {
        variable_names = realloc(variable_names, (param_count + 1) * sizeof(char));
        variable_names[param_count - 1] = line[0];

        variable_names[param_count] = '\0';
        variable_flag = 0;
      }
    }
  }

  /***************************************************************/
  /*             Storing configuration of environment            */
  /***************************************************************/
  (*env)->parameters = parameters;
  (*env)->count_of_parameters = param_count;
  (*env)->intervals = intervals;
  (*env)->variable_names = variable_names;
  (*env)->executable = executable;


  (*env)->meta_data_file = malloc((strlen(file_name) + 1 ) * sizeof(char));
  if ((*env)->meta_data_file == NULL){
    printf("Malloc failed\n");
    return;
  }

  sprintf((*env)->meta_data_file, "%s", file_name);

  parameters = NULL;
  fclose(file_pointer); /*close file */
  remove("gen.txt"); /*remove previous logs */
  remove("val.txt"); /*remove previous logs */
}

void remove_alpha_tags(creature *population) {
	creature *pointer = population;
	int continue_while;
	continue_while = 1;
	do  {
		pointer->is_alpha = 0;
		if (pointer->next) {
			pointer = pointer->next;
		} else {
			continue_while = 0;
		}
	} while (continue_while);


}

/*
* Returns pointer to last creature in population
*
* param  (creature *) population pointer to population
*
* returns creature * pointer to last creature in population
*/
creature *get_last_creature_in_list(creature *population) {
	creature *pointer;
	pointer = population;
	while (pointer->next) {
		pointer = pointer->next;
	}

	return pointer;
}

/*
* Finds creature with highest fitness and logs it to gen.txt file
*
* param  (creature *) population pointer to population
* param  int generation_number generation number
* param  (environment *) env configuration
*
* returns void
*/
void log_fittest(creature *population,int generation_number, environment *env){
		creature *pointer;
		creature *fittest;
		char log[BUFSIZE];
		int i;
		FILE * file_pointer;
		int continue_while;

		continue_while = 1;
		pointer        = population;
		fittest        = population;
		file_pointer   = fopen("gen.txt", "a");
		if (file_pointer == NULL) {
			/* Unable to open file hence exit */
			printf("Unable to open file.\n");
			exit(0);
		}

		do {
				if (pointer->fitness > fittest->fitness) {
						fittest = pointer;
				}

				if (pointer->next) {
					pointer = pointer->next;
				} else {
					continue_while = 0;
				}

		} while (continue_while);

		fittest->is_alpha = 1;

		printf("..Alpha creature has fitness:\t\t%f  \n\n", fittest->fitness); /*Comment tag */

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

/*
* Logs given creature to val.txt file
*
* param  (creature *) individual creature to log
* param  (environment *) env configuration
*
* returns void
*/
void log_results(creature * individual, environment * env){
  FILE * file_pointer;
	char log[BUFSIZE];
	int i;

	file_pointer = fopen("val.txt", "a");
	if (file_pointer == NULL) {
		/* Unable to open file hence exit */
		printf("\nUnable to open file.\n");
		exit(0);
	}

	sprintf(log, "%f\n", individual->fitness);
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

/*
* prints info about all members of population
*
* param  (creature *) population pointer to population
*
* returns void
*/
void print_population(creature *population){
	creature *pointer = population;
	int continue_while;
	int count;
	printf("Printing population \n"); /*Comment tag */
	continue_while = 1;
	count = 0;

		do  {
			printf("\nCreature \t  %d \n", count); /*Comment tag */
			printf("\t\t name %s \n", pointer->name); /*Comment tag */
			printf("\t\t fitness %f \n", pointer->fitness); /*Comment tag */
			printf("\t\t gene real %f \n", pointer->gene[0].real); /*Comment tag */
			printf("\t\t gene binary %d \n", pointer->gene[1].binary); /*Comment tag */
			count++;
			if (pointer->next) {
				pointer = pointer->next;
			} else {
				continue_while = 0;
			}
		} while (continue_while);
}

/*
* Iterates over population and counts the average population
*
* param  (creature *) population pointer to population
* param  int population_count population count
* param  (float *) average_fitness  pointer to average fitness, we store it here
*
* returns void
*/
void get_average_fitness(creature *population, int population_count, float *average_fitness){
	float sum = 0;
	creature *pointer = population;
	int i;

	/* sum of all fitnesses */
	for(i = 0; i < population_count; i++) {
		sum += pointer->fitness;
		pointer = pointer->next;
	}

	/*save average */
	*average_fitness = sum/(population_count);

}

/*
* Iterates over population until given index and returns pointer to creature
*
* param  (creature *) population pointer to population
* param  int index index of creature in population list
*
* returns void
*/
creature *get_creature_by_number(creature *population, int index) {
	int i;
	creature *pointer_to_creature = population;

	for (i = 0; i < index; i++) {
		pointer_to_creature = pointer_to_creature->next;
	}

	return pointer_to_creature;
}

/*
* Picks random creature with above average fitness and returns its index
*
* param  (creature *) population pointer to population
* param  int last_creature_index index of last creature
*
* returns int inde of valuable creature
*/
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

/*
* Writes data from creature's gene into metadata file,
* so we can test the creature
*
* param  (creature *) population pointer to population
* param  (environment *) env configuration
*
* returns void
*/
void write_creature_metadata(creature *creature, environment *env){
  FILE * meta_data_file_pointer;
  FILE * temporary_file_pointer;
  char variable_name[BUFSIZE];
  char interval[BUFSIZE];
  char new_line[BUFSIZE];
  char buffer[BUFSIZE];
  char tmp[BUFSIZE];
  char type;
  int variable_flag;
  int variable_count;
  int is_valid;

  variable_count = 0;
  variable_flag  = 0;
  is_valid       = 0;

  /*  Open both required files */
  meta_data_file_pointer = fopen(env->meta_data_file, "r");
  temporary_file_pointer = fopen("replace_tmp.txt", "w");

  /* fopen() return NULL if unable to open file in given mode. */
  if (meta_data_file_pointer == NULL || temporary_file_pointer == NULL) {
    /* Unable to open file hence exit */
    printf("\nUnable to open one of files.\n");
    printf("Please check whether file exists and you have read/write privilege.\n");
    exit(0);
  }

  /*
  * Read line from source file and write to destination
  * file after modifying given line according to creature's gene.
  */
  while ((fgets(buffer, BUFSIZE, meta_data_file_pointer)) != NULL) {

    /* If current line is line to replace */
    if (buffer[0] == '#' && buffer[2] == '(') {
      fputs(buffer, temporary_file_pointer);
      variable_flag = 1;
    } else if(variable_flag == 1 && buffer[0] != '#') {
      /*overwrite */

      type = env->parameters[variable_count];
      strcpy(interval, env->intervals[variable_count]);

      memset(tmp, 0, sizeof tmp);
      memset(variable_name, 0, sizeof variable_name);

      sscanf(buffer, "%s = %s", variable_name, tmp);
      memset(tmp, 0, sizeof tmp);

      if (type == VARIABLE_TYPE_INTEGER) {
        is_valid = is_valid_int(interval, creature->gene[variable_count].binary);
      } else if (type == VARIABLE_TYPE_REAL) {
        is_valid = is_valid_float(interval, creature->gene[variable_count].real);
      }

      if (!is_valid) {
        printf("Variable is not valid!\n");
        exit(0);
      }

      if (type == VARIABLE_TYPE_INTEGER) {
        sprintf(new_line, "%s = %d\n", variable_name, creature->gene[variable_count].binary);
      } else if (type == VARIABLE_TYPE_REAL) {
        sprintf(new_line, "%s = %f\n", variable_name, creature->gene[variable_count].real);
      }

      fputs(new_line, temporary_file_pointer);

      memset(new_line, 0, sizeof new_line);
      variable_count++;
      variable_flag = 0;

    }
    else {
      fputs(buffer, temporary_file_pointer);
      variable_flag = 0;
    }
    memset(buffer, 0, sizeof buffer);

  }

  fclose(temporary_file_pointer);
  /* Close all files to release resource */
  fclose(meta_data_file_pointer);

  /* Delete original source file */
  remove(env->meta_data_file);

  /* Rename temporary file as original file */
  rename("replace_tmp.txt", env->meta_data_file);
}

/*
* Read line with interval and type and store the interval and type
*
* param  (char *) line line with interval and type
* param  (char *) interval interval
* param  (char *) type pointer to type character
* param  (environment *) env configuration
*
* returns void
*/
void store_variable_from_line(char *line, char *interval, char *type) {
  char * new_line_char_position;
  float from_real;
  float to_real;
  int from_int;
  int to_int;

  if ((new_line_char_position=strchr(line, '\n')) != NULL){
    *new_line_char_position = '\0';
  }

  sscanf( line, "#_(%f,%f);%c", &from_real, &to_real, type );

  if (*type == VARIABLE_TYPE_INTEGER) {
    sscanf( line, "#_(%d,%d);Z", &from_int, &to_int );
    sprintf(interval, "%d,%d%c", from_int, to_int, '\0');

  } else if (*type == VARIABLE_TYPE_REAL) {
    sprintf(interval, "%f,%f%c", from_real, to_real, '\0');
  } else {
    printf("Unknown variale\n");
  }
}

/*
* Converts integer to its binary representation
* and stores it as string
*
* param  int n number to convert to binary
* param  (char **) result pointer to string to store to
*
* returns void
*/
void get_binary_from_int(int n, char ** result) {
  unsigned int mask;
  unsigned int u;
  int n_bits;
  int i;

  /* get count of bits we need (sizeof returns bytes) */
  n_bits = sizeof(n) * 8;

  (*result) = malloc((n_bits+1) * sizeof(char) );
  if ((*result) == NULL){
    printf("Malloc failed\n");
    return;
  }

  (*result)[n_bits] = '\0';

  u = *(unsigned int*)&n;

  mask = 1 << (n_bits-1); /* fill in values right-to-left */
  for (i = 0; i < n_bits; i++, mask >>= 1)
    (*result)[i] = ((u & mask) != 0) + '0';
}

/*
* Converts and returns integer from long number
* of ones and zeros representing binary number
*
* param  long n number with 0's and 1's to convert to integer
*
* returns int converted number
*/
int get_int_from_binary(long n){
	int decimal;
  int remainder;
  int i;

  decimal = 0;
  i       = 0;

	while (n!=0) {
		remainder = n%10;
		n        /= 10;
		decimal  += remainder * pow(2,i);
		++i;
	}

	return decimal;
}

/*
* returns 0 if value is outside of intervals
* returns something else than 0 if value is in interval
*
* param  (char *) interval interval
* param  int value number value to check
*
* returns int true/false
*/
int is_valid_int(char *interval, int value){
  int from;
  int to;
  sscanf(interval, "%d,%d", &from, &to);

  return (from <= value && value <= to);
}

/*
* returns 0 if value is outside of intervals
* returns something else than 0 if value is in interval
*
* param  (char *) interval interval
* param  float value number value to check
*
* returns int true/false
*/
int is_valid_float(char *interval, float value){
  float from;
  float to;

  sscanf(interval, "%f,%f", &from, &to);
  return (from <= value && value <= to);
}
