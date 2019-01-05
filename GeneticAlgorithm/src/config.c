#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../lib/structures.h"
#include "../lib/config.h"
#include "../lib/nature.h"

/* reads file with meta data and stores configuration to env */
/* env->executable */
/*      meta_data_file */
/*      count_of_parameters */
/*      parameters */
/*      intervals */
void get_environment(char* file_name, environment **env) {
  char **new_intervals;
  char **intervals;
  char *variable_names;
  char *executable;
  char *parameters;
  char *interval;
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

/* Writes data from creature's gene into metadata file, */
/* so we can test the creature */
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

/* Read line with interval and type and store the interval and type */
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

/* Converts integer to its binary representation */
/* and stores it as string                       */
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

/* returns integer from long long number */
/* of ones and zeros representing binary number */
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


/* returns 0 if value is outside of intervals */
/* returns something else than 0 if value is in interval */
int is_valid_int(char *interval, int value){
  int from;
  int to;
  sscanf(interval, "%d,%d", &from, &to);

  return (from <= value && value <= to);
}

/* returns 0 if value is outside of intervals */
/* returns something else than 0 if value is in interval */
int is_valid_float(char *interval, float value){
  float from;
  float to;

  sscanf(interval, "%f,%f", &from, &to);
  return (from <= value && value <= to);
}
