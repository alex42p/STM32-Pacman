#include "stm32f0xx.h"
#include <stdint.h>

uint8_t col = 0;
char current_char = 'A';


void enable_ports_keypad(){

    //assuming we want to use the same ports for the keypad--to be confirmed

    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    GPIOC->MODER &= ~(0x0000ffff);
    GPIOC->MODER |= 0x00005500;

    GPIOC->PUPDR &= ~(0x000000ff);
    GPIOC->PUPDR |= 0x000000aa;

}

void drive_column(int c) {
    
    c = c & 0x3;  
    GPIOC->BSRR = 0x00f00000;
    GPIOC->BSRR = 1 << (c + 4);
  
}
  
int read_rows() {
  
    int row = GPIOC->IDR & 0xf;
    return row;
  
}

//rows_to_key will read whether A or B was pressed, returns +1 or -1 respectively

char rows_to_key(int rows) {
  
    int store;
    char c;
  
  //only care about col 4 and rows 1 and 2

    store = col & 0x3;
  
    if(rows & 0x1){
      c = 'A';
    }
    else if(rows & 0x2){
      c = 'B';
    }
    else{
      c = '0';
    }
    return c;
  
  }

//handler function to take in the value from rows_to_key and update the display variable accordingly

void handle_key(char key) {
  if (key == 'A') {
      // Rotate forward
      current_char++;
      if (current_char > 'Z') {
          current_char = 'A';
      }
  }
  else if (key == 'B') {
      // Rotate backward
      current_char--;
      if (current_char < 'A') {
          current_char = 'Z';
      }
  }

  // Update display (e.g., leftmost column)
  disp[0] = current_char;
}

//will need to include a init_tim and handler to update the display as the display variable is changed

void TIM7_IRQHandler(){

  TIM7->SR &= ~TIM_SR_UIF;

  int rows = read_rows();
  if (rows != 0) {
    int key = rows_to_key(rows);
    handle_key(key);
  }

  char c  = disp[col];

  show_char(col, c);
  col++;
  if (col > 7){
    col = 0;
  }
  drive_column(col);

}

//initialize 
void setup_tim7() {
    
  RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
  TIM7->PSC = 48000 - 1;
  TIM7->ARR = 50 - 1;
  TIM7->DIER |= TIM_DIER_UIE;
  NVIC->ISER[0] = (1 << 18);
  TIM7->CR1 |= TIM_CR1_CEN;

}

//write display