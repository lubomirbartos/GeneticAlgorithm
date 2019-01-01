
#ifndef STRUCTURES_H
#define STRUCTURES_H

#define VARIABLE_TYPE_INTEGER 'Z'
#define VARIABLE_TYPE_REAL 'R'

typedef struct Configuration {
	char *executable;
	char *meta_data_file;
	int count_of_parameters;
	char *parameters;
	char **intervals;
} environment;


typedef union Data {
   int binary;
   float real;
} gene;

typedef struct Creature {
	char name[5];
	gene *gene;
	int is_alpha; //fittest of them all, the chosen one, invulnerable until fitter creature appears
	float fitness;
	struct Creature *next;
	struct Creature *previous;
} jedinec;


#endif
