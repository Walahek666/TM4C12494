#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************************/
// GLOBALS
volatile uint32_t riseTime = 0;
volatile uint32_t fallTime = 0;
volatile uint32_t echoTime = 0;

/*******************************************************************************************/
// SIMPLE DELAY (US LEVEL APPROX)
void delay_us(int us) {
    for(int i = 0; i < us * 12; i++);
}

/*******************************************************************************************/
// TIMER0 = FREE RUNNING MICROSECOND COUNTER
void timer0_init(void) {

    SYSCTL_RCGCTIMER_R |= 0x01;
    while(!(SYSCTL_PRTIMER_R & 0x01));

    TIMER0_CTL_R &= ~0x01;        // disable
    TIMER0_CFG_R = 0x00;          // 32-bit timer
    TIMER0_TAMR_R = 0x02;         // periodic mode

    TIMER0_TAILR_R = 0xFFFFFFFF;   // free run max
    TIMER0_TAPR_R = 119;          // 1 MHz tick (120MHz / 120)

    TIMER0_CTL_R |= 0x01;         // enable
}

/*******************************************************************************************/
// GPIO INIT (FIXED PIN USAGE)
// TRIG = PD0
// ECHO = PD1 (simple polling version for reliability)
void gpio_init(void) {

    SYSCTL_RCGCGPIO_R |= (1 << 3);   // Port D
    while(!(SYSCTL_PRGPIO_R & 0x08));

    // TRIG (PD0)
    GPIO_PORTD_AHB_DEN_R |= 0x01;
    GPIO_PORTD_AHB_DIR_R |= 0x01;

    // ECHO (PD1)
    GPIO_PORTD_AHB_DEN_R |= 0x02;
    GPIO_PORTD_AHB_DIR_R &= ~0x02;
}

/*******************************************************************************************/
// TRIGGER PULSE
void trigger_pulse(void) {

    GPIO_PORTD_AHB_DATA_R &= ~0x01;
    delay_us(5);

    GPIO_PORTD_AHB_DATA_R |= 0x01;
    delay_us(10);

    GPIO_PORTD_AHB_DATA_R &= ~0x01;
}

/*******************************************************************************************/
int main(void) {

    uint32_t start, stop;
    int i = 0;

    gpio_init();
    timer0_init();

    while(1) {

        /******** SEND TRIGGER ********/
        trigger_pulse();

        /******** WAIT FOR ECHO HIGH ********/
        while(GPIO_PORTD_AHB_DATA_R & 0x02 == 0);

        start = TIMER0_TAR_R;

        /******** WAIT FOR ECHO LOW ********/
        while(GPIO_PORTD_AHB_DATA_R & 0x02);

        stop = TIMER0_TAR_R;

        /******** CALCULATE PULSE WIDTH ********/
        echoTime = start - stop;

        float distance = (echoTime * 0.0343f) / 2.0f;

        printf("\nCycle: %d", i++);
        printf("\nEcho time: %d us", echoTime);
        printf("\nDistance: %.2f cm", distance);
    }
    
}