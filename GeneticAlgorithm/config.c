#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "structures.h"
#include "jedinec.h"
#include "config.h"
#include "nature.h"

//reads file with meta data and stores configuration to env
void get_environment(char* file_name, environment **env) {
  char *executable, *parameters, **intervals, **new_intervals, type[1], *interval, *line;
  int param_count = 0, i;
  FILE * file_pointer;
  size_t len = 0;
  ssize_t read;
  char *new_line_char_position;

  parameters = malloc(sizeof(char));
  if (parameters == NULL){
    printf("Malloc failed\n");
    return;
  }

  parameters[0] = '\0';

  srand(time(NULL));

  file_pointer = fopen(file_name, "r");
  if (file_pointer == NULL) {
    exit(EXIT_FAILURE);

  }

  line = malloc(100*sizeof(char));

  while ((read = getline(&line, &len, file_pointer)) != -1) {
    if (strlen(line) <= 2) {
      continue;
    }

    // for (i = 0; i < param_count; i++) //from one, we will free first with free below
    //   free(new_intervals[i]);

    // free(new_intervals);


    new_intervals = (char**)calloc(param_count, sizeof(char *));
    if (new_intervals == NULL){
      printf("Malloc failed\n");
      return;
    }

    for (i = 0; i < param_count; i++) {
      new_intervals[i] = malloc(strlen(intervals[i]) * sizeof(char) + 1);
      if (new_intervals[i] == NULL){
        printf("Malloc failed\n");
        return;
      }

      new_intervals[i][0] = '\0';
      strcpy(new_intervals[i], intervals[i]);
    }

    intervals = new_intervals;

    // file or variable, constants dont matter
    if (line[0] == '#') {

      if (strstr(line,  "#_(") != NULL) {
        // variable
        store_variable_from_line(line, interval, type);

        parameters = (char *) realloc(parameters, (strlen(parameters) + 1) * sizeof(char));
        if (parameters == NULL){
          printf("Malloc failed\n");
          return;
        }

        append_string(&parameters, type);

        intervals[param_count] = "\0";
        append_string(intervals + param_count, interval);
        interval[0] = '\0';

        param_count++;
      } else {
        // file

        executable = strdup(line+2); // prvni dva znaky ne TODO osetrit zda se to neposralo
        if ((new_line_char_position=strchr(executable, '\n')) != NULL)
            *new_line_char_position = '\0';

      }
    }
    free(line);
  }

  *env = malloc(sizeof(environment));
  if (*env == NULL){
    printf("Malloc failed\n");
    return;
  }

  (*env)->parameters = malloc((strlen(parameters) + 1 ) * sizeof(char));
  if ((*env)->parameters == NULL){
    printf("Malloc failed\n");
    return;
  }

  sprintf((*env)->parameters, "%s", parameters);

  (*env)->count_of_parameters = param_count;
  // (*env)->intervals = malloc((strlen(intervals) + 1 )*sizeof(char *));
  (*env)->intervals  = realloc(intervals, sizeof(intervals));
  if ((*env)->intervals == NULL){
    printf("Malloc failed\n");
    return;
  }


  (*env)->executable = malloc((strlen(executable) + 1 ) * sizeof(char));
  if ((*env)->executable == NULL){
    printf("Malloc failed\n");
    return;
  }

  sprintf((*env)->executable, "%s", executable);
  (*env)->meta_data_file = malloc((strlen(file_name) + 1 ) * sizeof(char));
  if ((*env)->meta_data_file == NULL){
    printf("Malloc failed\n");
    return;
  }

  sprintf((*env)->meta_data_file, "%s", file_name);

  fclose(file_pointer);
  if (line){
    free(line);
  }
  free(executable);
  free(parameters);


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

// Writes data from creatures gene into metadata file,
// so we can evaluate the creature
void write_creature_metadata(jedinec *creature, environment *env){
  /* File pointer to hold reference of input file */
  FILE * fPtr;
  FILE * fTemp;
  int BUFFER_SIZE = 1000;
  int i;
  char buffer[BUFFER_SIZE];
  char newline[BUFFER_SIZE];

  /*  Open all required files */
  fPtr  = fopen(env->meta_data_file, "r");
  fTemp = fopen("replace_tmp.txt", "w");

  /* fopen() return NULL if unable to open file in given mode. */
  if (fPtr == NULL || fTemp == NULL)
  {
    /* Unable to open file hence exit */
    printf("\nUnable to open file.\n");
    printf("Please check whether file exists and you have read/write privilege.\n");
    exit(EXIT_SUCCESS);
  }

  /*
  * Read line from source file and write to destination
  * file after replacing given line.
  */
  int variable_flag = 0;
  int variable_count = 0;
  char type;
  char *interval;
  char * newLine;
  char *value;
  char *variable_name = malloc(10 * sizeof(char));
  if (variable_name == NULL){
    printf("Malloc failed\n");
    return;
  }

  char *tmp = malloc(100 * sizeof(char));
  if (tmp == NULL){
    printf("Malloc failed\n");
    return;
  }

  int is_valid;

  while ((fgets(buffer, BUFFER_SIZE, fPtr)) != NULL)
  {

    /* If current line is line to replace */
    if (buffer[0] == '#' && buffer[2] == '(') {
      fputs(buffer, fTemp);
      variable_flag = 1;


    } else if(variable_flag == 1 && buffer[0] != '#') {
      //overwrite
      type = env->parameters[variable_count];
      interval = env->intervals[variable_count];

      sscanf(buffer, "%s = %s", variable_name, tmp);
      free(tmp);

      if (type == VARIABLE_TYPE_INTEGER) {
        is_valid = is_valid_int(interval, creature->gene[variable_count].binary);

      } else if (type == VARIABLE_TYPE_REAL) {
        is_valid = is_valid_float(interval, creature->gene[variable_count].real);

      }

      if (!is_valid) {
        exit;
      }


      newLine = malloc((strlen(variable_name) + 30) * sizeof(char));
      if (newLine == NULL){
        printf("Malloc failed\n");
        return;
      }


      if (type == VARIABLE_TYPE_INTEGER) {
        sprintf(newline, "%s = %d\n", variable_name, creature->gene[variable_count].binary);
      } else if (type == VARIABLE_TYPE_REAL) {
        sprintf(newline, "%s = %f\n", variable_name, creature->gene[variable_count].real);
      }

      fputs(newline, fTemp);

      free(newLine);
      free(variable_name);
      variable_count++;
      variable_flag = 0;
    }
    else {
      fputs(buffer, fTemp);
      variable_flag = 0;
    }
  }


  /* Close all files to release resource */
  fclose(fPtr);
  fclose(fTemp); // TODO malloc_consolidate error


  /* Delete original source file */
  remove(env->meta_data_file);

  /* Rename temporary file as original file */
  rename("replace_tmp.txt", env->meta_data_file);
}

void store_variable_from_line(char *line, char *interval, char *type) {
  float from_real, to_real;
  int from_int, to_int;

  sscanf( line, "#_(%f,%f);%c", &from_real, &to_real, type );

  if (*type == VARIABLE_TYPE_INTEGER) {
    sscanf( line, "#_(%d,%d);Z", &from_int, &to_int );
    sprintf(interval, "%d,%d", from_int, to_int);

  } else if (*type == VARIABLE_TYPE_REAL) {
    sprintf(interval, "%f,%f", from_real, to_real);
  } else {
    printf("Unknown variale\n");
  }
}


void append_string(char **text, char *appended_string) {
  char * result ;
  int size = 1; //last char is zero
  if (*text) {
    size += strlen(*text);
  }

  if (appended_string) {
    size += strlen(appended_string);
  }

  if((result = malloc(size * sizeof(char))) != NULL){

    result[0] = '\0';   // ensures the memory is an empty string
    if (*text) {
      strcat(result,*text);
    }

    strcat(result,appended_string);
    // free(*text);

    *text = malloc(sizeof(char) * strlen(result));
    if (*text == NULL){
      printf("Malloc failed\n");
      return;
    }

    *text[0] = '\0';   // ensures the memory is an empty string
    strcat(*text,result);
  } else {

    printf("malloc failed in append_string!\n");
  }
  if (result) {
    free(result);
  }
}

void get_binary_from_int(int n, char ** result) {
  unsigned int u, mask;
  int nbits, i;

  // determine the number of bits needed ("sizeof" returns bytes)
  nbits = sizeof(n) * 8;
  // free(*result);

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

  // result = NULL;

}
