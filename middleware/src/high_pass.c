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

void high_pass_init(float alpha, float scale, high_pass_st filter)
{
	filter->alpha = alpha;
	filter->scale = scale;

	filter->prev_output = 0.0;
}

//y[n]=x[n]−SMA[n]
//The output is equal to the input - the moving average
float high_pass(high_pass_st filter, float input)
{
	float output =
		filter->scale(input - (filter->alpha * input +
				       (1 - filter->alpha) * prev_output));

	filter->prev_output =
		filter->alpha * input + (1 - filter->alpha) * prev_output;

	return output;
}
