
#ifndef STRUCTURES_H
#define STRUCTURES_H


typedef struct Configuration {
	char *executable;
	char *meta_data_file;
	int count_of_parameters;
	char *parameters;
	char **intervals;
} environment;

typedef struct Jedinec {
	char *gene;
	int fitness;
	struct Jedinec *next;
	struct Jedinec *previous;
} jedinec;



#endif
