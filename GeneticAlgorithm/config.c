#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "jedinec.h"
#include "config.h"
#include "nature.h"

//reads file with meta data
void get_environment(char* file_name, environment **env) {

  char *executable;
	char *parameters;
  char **intervals;
  char **new_intervals;
	int param_count = 0;
	char *konstanta;
	char *type;
	char *interval;
  int INTERVAL_SIZE = 30;
  int i;

    FILE * file_pointer;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    file_pointer = fopen(file_name, "r");
    if (file_pointer == NULL) {
      exit(EXIT_FAILURE);

    }

    while ((read = getline(&line, &len, file_pointer)) != -1) {

      new_intervals = (char**)calloc(param_count, param_count * sizeof(char *));
      for (i = 0; i < param_count; i++) {
        new_intervals[i] = malloc(strlen(intervals[i]) * sizeof(char) + 1);
        new_intervals[i][0] = '\0';
        strcpy(new_intervals[i], intervals[i]);
      }
      intervals = new_intervals;

    	if (line[0] != '#') {

    	// konstanta
    		// store_const_from_line(line, &konstanta);
    		// append_string(&parameters, konstanta);
    		// param_count++;
    	} else {
    	// file or variable

      if (strstr(line,  "#_(") != NULL) {
    		// variable

    			store_variable_from_line(line, interval, type);
	    		append_string(&parameters, type);
          intervals[param_count] = "";
	    		append_string(intervals + param_count, interval);
          printf("\n parameters = %s\n", intervals[param_count]);

    			param_count++;

    		} else {
    		// file

    			executable = strdup(line+2); // prvni dva znaky ne TODO osetrit zda se to neposralo
    		}

    	}


        printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);



    }

    *env = malloc(sizeof(environment));
    // (*env)->parameters = malloc(strlen(parameters)*sizeof(char *));
    (*env)->parameters = strdup(parameters);
    (*env)->count_of_parameters = param_count;
    // (*env)->intervals = malloc(strlen(intervals)*sizeof(char *));
    (*env)->intervals  = intervals;
    // (*env)->executable = malloc(strlen(executable)*sizeof(char *));
    (*env)->executable = strdup(executable);
    (*env)->meta_data_file = strdup(file_name);

    fclose(file_pointer);
    if (line){
        free(line);
        free(parameters);
        free(intervals);
        free(executable);
    }
}
int is_valid_value(char type, char *interval, char *value){
    int from_int, to_int, val_int;
    float from_real, to_real, val_real;

    switch(type) {
      case VARIABLE_TYPE_INTEGER:
          from_int = atoi(strtok(interval, ","));
          to_int = atoi(strtok(interval, ","));
          val_int = atoi(value);
          return (from_int < val_int && val_int < to_int);
          break;
      case VARIABLE_TYPE_REAL:
          from_real = atoi(strtok(interval, ","));
          to_real = atoi(strtok(interval, ","));
          val_real = atof(value);
          return (from_real < val_real && val_real < to_real);
          break;
    }
}


void write_creature_metadata(jedinec *creature, environment *env){
  /* File pointer to hold reference of input file */
FILE * fPtr;
FILE * fTemp;
int BUFFER_SIZE = 200;
int i;
char buffer[BUFFER_SIZE];
char newline[BUFFER_SIZE];
char *gene = creature->gene; //strcpy TODO

/*  Open all required files */
fPtr  = fopen(env->meta_data_file, "r");
fTemp = fopen("replace.tmp", "w");

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
char *value;
while ((fgets(buffer, BUFFER_SIZE, fPtr)) != NULL)
{
    /* If current line is line to replace */
    if (buffer[0] == '#' && buffer[2] == '(') {
      variable_flag = 1;

    } else if(variable_flag == 1 && buffer[0] != '#') {
      //overwrite
      type = env->parameters[variable_count];
      interval = env->intervals[variable_count];

      char *variable_name = strtok(buffer, " = ");
      value = strtok(gene, ";");

      int is_valid = is_valid_value(type, interval, value);

      if (!is_valid) {
          exit;
      }

      strcpy(newline, variable_name);
      strcpy(newline, " = ");
      strcpy(newline, value);

      fputs(newline, fTemp);
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
fclose(fTemp);


/* Delete original source file */
remove(env->meta_data_file);

/* Rename temporary file as original file */
rename("replace.tmp", env->meta_data_file);

printf("\nSuccessfully replaced file");

}


void store_const_from_line(char *line, char **konstanta) {
    **konstanta = *line;
}

void store_variable_from_line(char *line, char *interval, char *type) {

    char * interval_local = strtok(line, ";");
    char *type_token = strtok(0, ";");
    *type = type_token[0];
    char *result;

    strtok(interval_local, "(");// get #_ out
    char * num_interval = strtok(interval_local, ")");

    if (*type_token == VARIABLE_TYPE_INTEGER) {
        int from = atoi(strtok(num_interval, ","));
        int to = atoi(strtok(num_interval, ","));
        result = "%d,%d";
        sprintf(interval, result, from, to);

    } else if (*type_token == VARIABLE_TYPE_REAL) {

        float from = atof(strtok(num_interval, ","));

        float to = atof(strtok(num_interval, ","));
        result = "%f,%f";
        sprintf(interval, result, from, to);


    } else {
        printf("Unknown variale");
    }

}


void append_string(char **text, char *appended_string) {
  printf("\n bitch \n");

  // printf("\n strlen(*text) = %ld\n", strlen(*text));
  printf("\n strlen(*text) = %s\n", *text);
  printf("\n bitch \n");


    char * result ;
    if((result = malloc(strlen(*text)+strlen(appended_string)+1)) != NULL){

        result[0] = '\0';   // ensures the memory is an empty string
        strcat(result,*text);

        strcat(result,appended_string);
        *text = malloc(sizeof(char) * strlen(result));
        *text[0] = '\0';   // ensures the memory is an empty string
        strcat(*text,result);



    } else {

        printf("malloc failed in append_string!\n");
    }
}

char *get_binary_from_int(int n) {
    unsigned int u, mask;
    int nbits, i;
    char *s;

    // determine the number of bits needed ("sizeof" returns bytes)
    nbits = sizeof(n) * 8;
    s = malloc(nbits+1);
    s[nbits] = '\0';

    u = *(unsigned int*)&n;

    mask = 1 << (nbits-1); // fill in values right-to-left
    for (i = 0; i < nbits; i++, mask >>= 1)
        s[i] = ((u & mask) != 0) + '0';
    return s;
}
