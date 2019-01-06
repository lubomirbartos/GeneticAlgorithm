#ifndef CONFIG_H
#define CONFIG_H
#include "structures.h"

creature *get_last_creature_in_list (creature *population);
creature *get_creature_by_number(creature *population, int index);

void store_variable_from_line(char *line, char *interval, char *type);
void write_creature_metadata(creature *creature, environment *env);
void get_binary_from_int(int n, char ** result);
void get_average_fitness(creature *population, int population_count, float *average_fitness);
void remove_alpha_tags(creature *population);
void print_population(creature *population);
void get_environment(char* file_name, environment **env);
void append_string(char **text, char *appended_string);
void log_fittest(creature *population,int generation_number, environment *env);
void log_results(creature *individual, environment *env);

int get_valuable_creature_index(creature *population, int last_creature_index);
int get_int_from_binary(long n);
int is_valid_float(char *interval, float value);
int is_valid_int(char *interval, int value);

#endif
