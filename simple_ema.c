#include <stdio.h>

// Exponential Moving Average Lowpass Filter 
void ema_filter(float current_value, float *previous_ema, float alpha) {
    *previous_ema = (alpha * current_value) + ((1 - alpha) * (*previous_ema));
}

int main() {
    float alpha = 0.1f; // smoothing strength
    float ema = 0.0f; // initial value can be 0
    // Something like : ema_filter(curent_unfiltered_value, &ema, alpha)
    return 0;
}
