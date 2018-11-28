#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "jedinec.h"

void get_average_fitness(jedinec *population, int *population_count, float *average_fitness){
	float sum = 0;

	// sum of all fitnesses
	for(int i = 0; i < *population_count; i++) {
		sum += population->fitness;
	}

	//save average
	*average_fitness = sum/(*population_count);
}
