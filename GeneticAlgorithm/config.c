#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
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
	char type[1];
	char *interval;
  int INTERVAL_SIZE = 30;
  int i;
  srand(time(NULL));

    FILE * file_pointer;
    char * line;
    line[0] = '\0';
    size_t len = 0;
    ssize_t read;
    parameters = malloc(sizeof(char));
    parameters[0] = '\0';


    file_pointer = fopen(file_name, "r");
    if (file_pointer == NULL) {
      exit(EXIT_FAILURE);

    }

    while ((read = getline(&line, &len, file_pointer)) != -1) {

      if (strlen(line) <= 2) {
          continue;
      }

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

            // size_t sajz = (strlen(parameters) + strlen(type)) * sizeof(char);
            parameters = (char *) realloc(parameters, (strlen(parameters) + 2) * sizeof(char));
            // printf("\n Type = %s\n", type);
  	    		append_string(&parameters, type);

            // printf("\n Parameters = %s\n", parameters);
            intervals[param_count] = "\0";
  	    		append_string(intervals + param_count, interval);
            interval[0] = '\0';
            // printf("\n Interval = %s\n", intervals[param_count]);

      			param_count++;

    		} else {
    		// file

    			executable = strdup(line+2); // prvni dva znaky ne TODO osetrit zda se to neposralo
    		}

    	}



        // printf("Retrieved line of length %zu \n", read);
        // printf("%s\n", line);



    }

    *env = malloc(sizeof(environment));
    // (*env)->parameters = malloc(strlen(parameters)*sizeof(char *));

    (*env)->parameters = strdup(parameters);
    (*env)->count_of_parameters = param_count;
    // (*env)->intervals = malloc(strlen(intervals)*sizeof(char *));
    (*env)->intervals  = realloc(intervals, sizeof(intervals));

    // (*env)->executable = malloc(strlen(executable)*sizeof(char *));
    (*env)->executable = strdup(executable);
    (*env)->meta_data_file = strdup(file_name);

    fclose(file_pointer);
    if (line){
        free(line);
        free(parameters);
        free(executable);
    }


}

int is_valid_int(char *interval, int value){
  int from, to;
  sscanf(interval, "%d,%d", &from, &to);
  return (from < value && value < to);

}

int is_valid_float(char *interval, float value){
  float from, to;
  sscanf(interval, "%f,%f", &from, &to);
  return (from < value && value < to);
}


void write_creature_metadata(jedinec *creature, environment *env){
  /* File pointer to hold reference of input file */
FILE * fPtr;
FILE * fTemp;
int BUFFER_SIZE = 200;
int i;
char buffer[BUFFER_SIZE];
char newline[BUFFER_SIZE];

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
char * newLine;
char *value;
char *variable_name = malloc(10 * sizeof(char));
char *tmp = malloc(100 * sizeof(char));
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
fclose(fTemp);


/* Delete original source file */
remove(env->meta_data_file);

/* Rename temporary file as original file */
rename("replace.tmp", env->meta_data_file);
}


void store_const_from_line(char *line, char **konstanta) {
  *konstanta = malloc(strlen(line - 4));
  *konstanta = line + 4;
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

  // printf("\n strlen(*text) = %ld\n", strlen(*text));
  // printf("\n text = %s\n", *text);
  // printf("\n appended_string = %s\n", appended_string);


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
