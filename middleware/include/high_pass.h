#ifndef HIGH_PASS_H
#define HIGH_PASS_H

#include <stdio.h>

typedef struct {
	//Function Parameters
	float alpha;
	float scale;

	float prev_output;
	float prev_input;

} high_pass_t;

/**
 * @brief Initiailzing the high pass filter with filter coefficient & desired scale
 * 
 * @param alpha filter coefficient controlling freq response
 * @param scale desired scaling for filter
 * @param filter pointer to a new high pass struct
 */
void high_pass_init(float alpha, float scale, high_pass_t *filter);

/**
 * @brief Function applying filter to a new sample, returning the filtered output
 * 
 * @param filter passing pointer to initialized high pass struct
 * @param input new sample to be filtered
 * @return float Filtered & Scaled output value based on prev values
 */
float high_pass(high_pass_t *filter, float input);

#endif
