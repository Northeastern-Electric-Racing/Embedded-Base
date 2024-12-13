#ifndef SIMPLE_EMA
#define SIMPLE_EMA

// Exponential Moving Average Lowpass Filter
ema_filter(float current_value, float *previous_ema, float alpha);

#endif /* SIMPLE_EMA */