
#ifndef STRUCTURES_H
#define STRUCTURES_H


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
	gene *gene;
	int fitness;
	struct Creature *next;
	struct Creature *previous;
} jedinec;


#endif
