#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************************/
// GLOBAL VARIABLES (Echo measurement)
volatile uint32_t riseTime = 0;   // Echo HIGH start time
volatile uint32_t fallTime = 0;   // Echo LOW end time
volatile uint32_t echoTime = 0;   // Pulse width

/*******************************************************************************************/
// SW Timer
void wait(int ticks) {
    for (int i = 0; i < ticks; i++);
}

// Sleep Function
void sleep(int ms) {

    for(int i = 0; i < ms-1; i++) {
        TIMER0_ICR_R = 0x01;        // Clear timeout flag
        TIMER0_CTL_R = 0x0001;      // Start Timer0A
        while (!(TIMER0_RIS_R & 0x01)); // Wait for timeout
        TIMER0_ICR_R = 0x01;        // Clear flag
    }
}

/*******************************************************************************************/
// Configure Timer0 (free running for timestamping)
void configure_timers() {

    SYSCTL_RCGCTIMER_R |= (0x1 << 0); // Enable Timer0
    while (!(SYSCTL_PRTIMER_R & 0x01));

    TIMER0_CTL_R &= ~0x01;     // Disable Timer0A
    TIMER0_CFG_R = 0x04;       // 16-bit mode
    TIMER0_TAMR_R = 0x02;      // Periodic mode
    TIMER0_TAPR_R = 123 - 1;   // Prescaler

    TIMER0_TAILR_R = 65000;    // Load value

    TIMER0_CTL_R |= 0x01;      // Enable Timer0A
}

/*******************************************************************************************/
// GPIO + Interrupt Config
void configure_ports() {

    SYSCTL_RCGCGPIO_R |= (0x1 << 3);   // Port D (Echo)
    SYSCTL_RCGCGPIO_R |= (0x1 << 12);  // Port M (Trigger)

    while (!(SYSCTL_PRGPIO_R & 0x0008));
    while (!(SYSCTL_PRGPIO_R & (0x1 << 12)));

    /******************* ECHO PIN (PD1 INPUT INTERRUPT) *******************/
    GPIO_PORTD_AHB_DEN_R |= 0x02;     // PD1 digital enable
    GPIO_PORTD_AHB_DIR_R &= ~0x02;    // input

    GPIO_PORTD_AHB_IS_R &= ~0x02;     // edge sensitive
    GPIO_PORTD_AHB_IBE_R &= ~0x02;    // single edge
    GPIO_PORTD_AHB_IEV_R |= 0x02;     // rising edge first
    GPIO_PORTD_AHB_ICR_R = 0x02;      // clear flag
    GPIO_PORTD_AHB_IM_R |= 0x02;      // enable interrupt

    /******************* TRIGGER PIN (PM0 OUTPUT) *******************/
    GPIO_PORTM_DEN_R |= 0x01;         // PM0 enable
    GPIO_PORTM_DIR_R |= 0x01;         // output
    GPIO_PORTM_DATA_R &= ~0x01;       // LOW

    /******************* NVIC ENABLE *******************/
    NVIC_EN0_R |= (1 << 3);           // Port D interrupt enable
}

/*******************************************************************************************/
// GPIO INTERRUPT HANDLER (Echo capture)
void GPIOPortD_Handler(void) {

    // Rising edge (Echo starts)
    if (GPIO_PORTD_AHB_RIS_R & 0x02) {

        riseTime = TIMER0_TAR_R;         // capture start time
        GPIO_PORTD_AHB_ICR_R = 0x02;     // clear flag

        GPIO_PORTD_AHB_IEV_R &= ~0x02;   // switch to falling edge
    }

    // Falling edge (Echo ends)
    else {

        fallTime = TIMER0_TAR_R;        // capture end time
        GPIO_PORTD_AHB_ICR_R = 0x02;    // clear flag

        echoTime = riseTime - fallTime;  // compute pulse width

        GPIO_PORTD_AHB_IEV_R |= 0x02;   // switch back to rising edge
    }
}

/*******************************************************************************************/
int main(void) {

    int i = 0;

    configure_ports();
    configure_timers();

    while(1) {

        /************ TRIGGER PULSE (HC-SR04) ************/
        GPIO_PORTM_DATA_R |= 0x01;   // HIGH trigger
        wait(50);                    // small delay (~10us equivalent)
        GPIO_PORTM_DATA_R &= ~0x01;  // LOW trigger

        /************ WAIT FOR ECHO TO BE MEASURED ************/
        sleep(10);

        /************ DISTANCE CALCULATION ************/
        float distance = (echoTime * 0.0343f) / 2.0f;

        printf("\nCounter: %d", i++);
        printf("\nEcho Time: %d us", echoTime);
        printf("\nDistance: %.2f cm", distance);
    }
}