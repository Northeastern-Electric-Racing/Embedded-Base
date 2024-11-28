#ifndef HIGH_PASS_H
#define HIGH_PASS_H

#include <stdio.h>

typedef struct {
	//Function Parameters
	float alpha;
	float scale;

	float prev_output;

} high_pass_t;

/**
 * @brief 
 * 
 * @param alpha 
 * @param scale 
 * @param filter 
 */
void high_pass_init(float alpha, float scale, high_pass_t *filter);

/**
 * @brief 
 * 
 * @param filter 
 * @param input 
 * @return float 
 */
float high_pass(high_pass_t *filter, float input);

#endif
