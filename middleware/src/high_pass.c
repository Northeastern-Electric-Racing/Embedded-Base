#include "high_pass.h"

void high_pass_init(float alpha, float scale, high_pass_t *filter)
{
	filter->alpha = alpha;
	filter->scale = scale;

	filter->prev_output = 0.0;
}

float high_pass(high_pass_t *filter, float input)
{
	float output =
		filter->scale(input - (filter->alpha * input +
				       (1 - filter->alpha) * prev_output));

	filter->prev_output =
		filter->alpha * input + (1 - filter->alpha) * prev_output;

	return output;
}
