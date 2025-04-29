// // pin macros for TFT LCD display
// #define CS_LOW    (GPIOB->BRR |= (1 << 8))
// #define CS_HIGH   (GPIOB->BSRR |= (1 << 8))
// #define DC_CMD    (GPIOB->BRR |= (1 << 14))  // D/C low = command
// #define DC_DATA   (GPIOB->BSRR |= (1 << 14)) // D/C high = data
// #define RST_LOW   (GPIOB->BRR |= (1 << 11))
// #define RST_HIGH  (GPIOB->BSRR |= (1 << 11))

#define CS_LOW    (GPIOA->BRR |= (1 << 15))
#define CS_HIGH   (GPIOA->BSRR |= (1 << 15))
#define DC_CMD    (GPIOA->BRR |= (1 << 6))  // PA6 for DC LOW
#define DC_DATA   (GPIOA->BSRR |= (1 << 6)) // PA6 for DC HIGH
#define RST_LOW   (GPIOA->BRR |= (1 << 3))
#define RST_HIGH  (GPIOA->BSRR |= (1 << 3))

