#include "stm32f0xx.h"
#include "i2c.h"
#include "high_score.h"

extern void internal_clock();
/**
 * TODO:
 * - test r/w with high_score/I2C code Alex wrote
 */
int main(void)
{
    internal_clock();   // Never comment this out!
    // Enable all peripheral ports
    enable_i2c_ports();

    // Setup next thing
}