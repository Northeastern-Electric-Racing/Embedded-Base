#include <stdio.h>
#include "simple_ema.h"

void ema_filter(float current_value, float *previous_ema, float alpha)
{
	*previous_ema =
		(alpha * current_value) + ((1 - alpha) * (*previous_ema));
}
