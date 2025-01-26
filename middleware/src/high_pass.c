#include "high_pass.h"

void high_pass_init(float alpha, float scale, high_pass_t *filter)
{
	filter->alpha = alpha;
	filter->scale = scale;

	filter->prev_output = 0.0;
	filter->prev_input = 0.0;
}

float high_pass(high_pass_t *filter, float input)
{
	float output =
		filter->scale * (filter->alpha * (input - filter->prev_input) +
				 (1 - filter->alpha) * filter->prev_output);

	filter->prev_input = input;
	filter->prev_output = output;

	return output;
}
