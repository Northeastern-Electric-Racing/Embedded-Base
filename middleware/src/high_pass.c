/*
Understanding High-Pass Filter Implementation

Goal is to customize the cutoff frequency/strength 

One style:
-alpha factor  -> smoothing factor used to make the changes smoother
-scale factor  -> scales the output given by algo
these two params correlate to strength

Or user could pass desired cutoff frequency and that would be used

Algorithm:
1. first possibility
output = alpha * last_output + (1-alpha) * input

output = alpha * last_output + alpha * (avg - prev_avg)

*/
#include "high_pass.h"

//Samples can be modified
#define SAMPLES 12

//Scale Var allows adjustment of output strength
//Alpha Var controls weight of prev input & sum difference
float high_pass(int alpha, int scale)
{
	static float buffer[SAMPLES] = { 0 }; //initialize changing buffer
	static int index = 0;
	static float prev_sum = 0.0;

	//buffer[index] = freq; //-> adding the freq input into the

	index = (index + 1) % SAMPLES;

	float sum = 0.0;

	//Iterates through all indices
	for (int i = 0; i < SAMPLES; i++) {
		sum += buffer[i];
	}

	//Algorithm
	//
	float output =
		scale * (alpha * buffer[index - 1] + alpha * (sum - prev_sum));

	prev_sum = sum; //updates the prev_sum

	return output;
}
