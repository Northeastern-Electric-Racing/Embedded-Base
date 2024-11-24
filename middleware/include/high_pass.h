#ifndef HIGH_PASS_H
#define HIGH_PASS_H

#include <stdio.h>

typedef struct {
	//Function Parameters
	float alpha;
	float scale;

	float prev_output;

} high_pass_st;

void high_pass_init(float alpha, float scale, high_pass_st filter);
/**
@brief Initialization for high pass values
 */

float high_pass(high_pass_st filter, float input);
/**
@brief Function for high pass filter
 */

#endif
