#ifndef CONFIG_H
#define CONFIG_H
#include "structures.h"

void get_environment(char* file_name, environment **env);
void store_const_from_line(char *line, char **konstanta);
void store_variable_from_line(char *line, char *interval, char *type);
void append_string(char **text, char *appended_string);
void get_binary_from_int(int n, char ** result);
void write_creature_metadata(creature *creature, environment *env);
int is_valid_int(char *interval, int value);
int is_valid_float(char *interval, float value);
int get_int_from_binary(long long n);

#endif
