#ifndef CONFIG_H
#define CONFIG_H
#include "structures.h"




void get_environment(char* file_name, environment **env);
void store_const_from_line(char *line, char **konstanta);
void store_variable_from_line(char *line, char *interval, char *type);
void append_string(char **text, char *appended_string);
char *get_binary_from_int(int n);
void write_creature_metadata(jedinec *creature, environment *env);
int is_valid_value(char type, char *interval, gene value);


#endif
