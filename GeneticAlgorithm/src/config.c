#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../lib/structures.h"
#include "../lib/config.h"
#include "../lib/nature.h"

// reads file with meta data and stores configuration to env
// env->executable
//      meta_data_file
//      count_of_parameters
//      parameters
//      intervals
void get_environment(char* file_name, environment **env) {
  char *executable, *parameters, **intervals, **new_intervals, type[2], *interval;
  int param_count = 0, i;
  FILE * file_pointer;
  ssize_t read = -1;
  char *new_line_char_position;
  int interval_size = 50;
  int BUFFER_SIZE = 1000;
  char line[BUFFER_SIZE];
  int variable_flag = 0;
  char *variable_names = malloc(sizeof(char));
  variable_names[0] = '\0';


  parameters = malloc(sizeof(char));
  if (parameters == NULL){
    printf("Malloc failed\n");
    return;
  }

  parameters[0] = '\0';

  srand(time(NULL));
  intervals = NULL;

  file_pointer = fopen(file_name, "r");

  if (file_pointer == NULL) {
    exit(EXIT_FAILURE);
  }

  //********************************************************//
  //   read configuration from metadata file line by line   //
  //********************************************************//

  while ((fgets(line, BUFFER_SIZE, file_pointer)) != NULL) {

    if (line && line[1] == '\n') {
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
        //********************************************************//
        //            Line with details of variable               //
        //********************************************************//
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
        //********************************************************//
        //              Line with executable file                 //
        //********************************************************//

        // if ((new_line_char_position=strchr(line, '\n')) != NULL){
        //   *new_line_char_position = ' ';
        // }

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
    // free(line);
    // line = malloc(100*sizeof(char));
  }
  // if (line) {
  //   free(line);
  // }


  //*************************************************************//
  //             Storing configuration of environment            //
  //*************************************************************//
  (*env)->parameters = parameters;
  (*env)->count_of_parameters = param_count;
  (*env)->intervals = intervals;
  (*env)->variable_names = variable_names;
  (*env)->executable = executable;


  // (*env)->executable = malloc((strlen(executable) + 1 ) * sizeof(char));
  // if ((*env)->executable == NULL){
  //   printf("Malloc failed\n");
  //   return;
  // }
  (*env)->meta_data_file = malloc((strlen(file_name) + 1 ) * sizeof(char));
  if ((*env)->meta_data_file == NULL){
    printf("Malloc failed\n");
    return;
  }


  // (*env)->executable[0] = '\0';
  // strcpy((*env)->executable, executable);
  // (*env)->executable[strlen(executable)] = '\0';


  sprintf((*env)->meta_data_file, "%s", file_name);
  // sprintf((*env)->executable, "%s", executable);



  parameters = NULL;
  // free(executable);
  fclose(file_pointer); //close file
  remove("../gen.txt"); //remove previous logs
  remove("../val.txt"); //remove previous logs
}

// Writes data from creature's gene into metadata file,
// so we can test the creature
void write_creature_metadata(creature *creature, environment *env){
  FILE * meta_data_file_pointer, * temporary_file_pointer;
  int BUFFER_SIZE = 1000, i, variable_flag = 0, variable_count = 0, is_valid = 0;
  char buffer[BUFFER_SIZE], type, interval[100], new_line[100], variable_name[20], tmp[100];

  /*  Open both required files */
  meta_data_file_pointer  = fopen(env->meta_data_file, "r");
  temporary_file_pointer = fopen("replace_tmp.txt", "w");

  /* fopen() return NULL if unable to open file in given mode. */
  if (meta_data_file_pointer == NULL || temporary_file_pointer == NULL)
  {
    /* Unable to open file hence exit */
    printf("\nUnable to open one of files.\n");
    printf("Please check whether file exists and you have read/write privilege.\n");
    exit(0);
  }

  /*
  * Read line from source file and write to destination
  * file after modifying given line according to creature's gene.
  */
  while ((fgets(buffer, BUFFER_SIZE, meta_data_file_pointer)) != NULL) {
    /* If current line is line to replace */
    if (buffer[0] == '#' && buffer[2] == '(') {
      fputs(buffer, temporary_file_pointer);
      variable_flag = 1;
    } else if(variable_flag == 1 && buffer[0] != '#') {
      //overwrite
      type = env->parameters[variable_count];
      strcpy(interval, env->intervals[variable_count]);

      memset(tmp, 0, sizeof tmp);
      memset(variable_name, 0, sizeof variable_name);

      sscanf(buffer, "%s = %s", variable_name, tmp);

      if (type == VARIABLE_TYPE_INTEGER) {
        is_valid = is_valid_int(interval, creature->gene[variable_count].binary);
      } else if (type == VARIABLE_TYPE_REAL) {
        is_valid = is_valid_float(interval, creature->gene[variable_count].real);
      }

      if (!is_valid) {
        exit;
      }

      if (new_line == NULL){
        printf("Malloc failed\n");
        return;
      }

      if (type == VARIABLE_TYPE_INTEGER) {
        sprintf(new_line, "%s = %d\n", variable_name, creature->gene[variable_count].binary);
      } else if (type == VARIABLE_TYPE_REAL) {
        sprintf(new_line, "%s = %f\n", variable_name, creature->gene[variable_count].real);
      }

      fputs(new_line, temporary_file_pointer);
      variable_count++;
      variable_flag = 0;

    }
    else {
      fputs(buffer, temporary_file_pointer);
      variable_flag = 0;
    }
  }

  fclose(temporary_file_pointer); // TODO malloc_consolidate error
  /* Close all files to release resource */
  fclose(meta_data_file_pointer);

  /* Delete original source file */
  remove(env->meta_data_file);

  /* Rename temporary file as original file */
  rename("replace_tmp.txt", env->meta_data_file);
}

// Read line with interval and type and store the interval and type
void store_variable_from_line(char *line, char *interval, char *type) {
  float from_real, to_real;
  int from_int, to_int;
  char * new_line_char_position;

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

// Converts integer to its binary representation
// and stores it as string
void get_binary_from_int(int n, char ** result) {
  unsigned int u, mask;
  int nbits, i;

  // determine the number of bits needed ("sizeof" returns bytes)
  nbits = sizeof(n) * 8;

  (*result) = malloc((nbits+1) * sizeof(char) );
  if ((*result) == NULL){
    printf("Malloc failed\n");
    return;
  }

  (*result)[nbits] = '\0';

  u = *(unsigned int*)&n;

  mask = 1 << (nbits-1); // fill in values right-to-left
  for (i = 0; i < nbits; i++, mask >>= 1)
    (*result)[i] = ((u & mask) != 0) + '0';
}

// returns integer from long long number
// of ones and zeros representing binary number
int get_int_from_binary(long long n){
	int decimal = 0, i = 0, remainder;
	while (n!=0) {
		remainder = n%10;
		n /= 10;
		decimal += remainder * pow(2,i);
		++i;
	}
	return decimal;
}


// returns 0 if value is outside of intervals
// returns something else than 0 if value is in interval
int is_valid_int(char *interval, int value){
  int from, to;
  sscanf(interval, "%d,%d", &from, &to);
  return (from < value && value < to);
}

// returns 0 if value is outside of intervals
// returns something else than 0 if value is in interval
int is_valid_float(char *interval, float value){
  float from, to;
  sscanf(interval, "%f,%f", &from, &to);
  return (from < value && value < to); // TODO this is not good I think
}
