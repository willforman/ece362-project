#include "stm32f0xx.h"
#include "display.h"
#include "dac.h"

Dir dir;
int playingSong = 0;
uint8_t pa1State;
uint8_t pa0State;

// Is for scrolling the display
void initDisplay(int calledEveryMs) {
    RCC->APB1ENR = RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 48 - 1;
    TIM6->ARR = 100 * calledEveryMs - 1;
    TIM6->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] |= 1 << TIM6_DAC_IRQn;
}

void initButtonScanning(int calledEveryMs) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->CR1 |= TIM_CR1_ARPE;
    TIM7->ARR = 48 - 1;
    TIM7->PSC = 100 * calledEveryMs - 1;
    TIM7->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] |= 1 << TIM7_IRQn;

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_1;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR2_1;
}

void enableDisplay() {
    dir.numFiles = 0;
    dir.path="/";
    updateFiles(&dir);
    TIM6->CR1 |=  TIM_CR1_CEN;
}

void disableDisplay() {
    TIM6->CR1 &= ~TIM_CR1_CEN;
    clearDisplay();
}

void enableButtonScanning() {
    TIM7->CR1 |=  TIM_CR1_CEN;
}

void disableButtonScanning() {
    TIM7->CR1 &= ~TIM_CR1_CEN;
}

void TIM6_IRQHandler() {
    TIM6->SR &= ~TIM_SR_UIF;
    if (!playingSong) {
        scrollDisplay(&dir);
    }
}

void TIM7_IRQHandler() { // invokes every 1ms to read from pa0 and pb2
    TIM7->SR &= ~TIM_SR_UIF;
    int pa0 = GPIOA->IDR & 0x1;
    int pa1 = GPIOA->IDR & 0x2;
    // if playing song
    if (playingSong) {
        // first button: play pause
        if (pa0) {
            togglePlay();
            return;
        }
        // second button: end song
        else if (pa1) {
            stop();
            playingSong = 0;
            return;
        }
    } else { // song selection
        // first button: select file
        if (pa0) {
            if (handleFileSelectButton(&dir)) {
                playingSong = 1;
                disableDisplay();
            }
            return;
        } 
        // second button: move the file to the next
        else if (pa1) {
            handleFileNextButton(&dir);
            return;
        }
    }
}