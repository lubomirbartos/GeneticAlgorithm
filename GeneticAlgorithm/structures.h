#ifndef STRUCTURES_H
#define STRUCTURES_H

#define VARIABLE_TYPE_INTEGER 'Z'
#define VARIABLE_TYPE_REAL 'R'
#define POPULATION_LIMIT 100
#define DEFAULT_MUTATION_RATE 5
#define BUFSIZE 200

typedef struct Configuration {
	char **intervals;        /* intervals for genes                          */
	char *meta_data_file;    /* name of metadata file                        */
	char *variable_names;    /* array of one-char variable names             */
	char *executable;        /* name of executable file                      */
	char *parameters;        /* array of one-char indicators of type of gene */
	int count_of_parameters; /* count of genes/variables                     */
} environment;

typedef union Data {
   int binary;
	 float real;
} gene;

typedef struct Creature {
	struct Creature *next;     /* pointer to next creature in list              */
	struct Creature *previous; /* pointer to previous creature in list          */
	gene *gene;                /* array of genes                                */
	int first;                 /* boolean value, 1 if creature is first in list */
	int last;                  /* boolean value, 1 if creature is last in list  */
	int is_alpha;              /* fittest of them all, the chosen one           */
	float fitness;             /* fitness of creature                           */
} creature;
#endif
