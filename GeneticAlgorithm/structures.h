#ifndef STRUCTURES_H
#define STRUCTURES_H

#define VARIABLE_TYPE_INTEGER 'Z'
#define VARIABLE_TYPE_REAL 'R'
#define POPULATION_LIMIT 100
#define DEFAULT_MUTATION_RATE 5
#define BUFSIZE 200


typedef struct Configuration {
	char **intervals;
	char *meta_data_file;
	char *variable_names;
	char *executable;
	char *parameters;
	int count_of_parameters;
} environment;


typedef union Data {
   int binary;
	 float real;
} gene;

typedef struct Creature {
	struct Creature *next;
	struct Creature *previous;
	char name[5];
	gene *gene;
	int first;
	int last;
	int is_alpha; /*fittest of them all, the chosen one, invulnerable until fitter creature appears*/
	float fitness;
} creature;


#endif
