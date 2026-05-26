/** GPIO Timers/Interrupts Project for TM4C1294ncpdt
 *
 * File:    main.c
 * Author: Bhathiya Ranasinghe
 * Date: 20.05.2026
 * Version: 1
 * 32vur ARM Cortex M4 120MHz Cpu
 * Blinks the onboard LEDs using a busy-wait loop as delay
 */

#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************************/
//SW Timer
void wait(int ticks) {
    for (int i = 0; i < ticks; i++);   
}

//Sleep Function
void sleep(int ms) {
 
    
    for(int i = 0; i < ms-1; i++) {
        TIMER0_ICR_R = 0x01; // Clearing Flag at 0x01 by Timer0 A Time-out
        TIMER0_CTL_R = 0x0001; // Start Timer0 A
        while (!(TIMER0_RIS_R & 0x01)); // Wait for the Raw Interrupt Status flag (TATORIS) to turn 1 
        TIMER0_ICR_R = 0x01;// Clear the flag by writing 1 to the Interrupt Clear Register 
    }
    
    
}

//Configure Timers
void configure_timers() {
    SYSCTL_RCGCTIMER_R |= (0x1 << 0); //activating Timer module 0
    while (!(SYSCTL_PRTIMER_R & 0x01));      // wait for clock to stabilize for the Timer
    TIMER0_CTL_R &= ~0x001;   // disable Timer0 A
    TIMER0_CFG_R = 0x0004; //16bit Mode in Timer 0, 0x0 32 bit mode, 0x1 RTC
    TIMER0_TAMR_R = 0x01; //Pin 5 is for Match enabled for Interrupts (0x22) and periodic
    // Timer AB Mode 0x1 One shot ox2 Periodic Timer
    //pin4 Up Mode  | (0x1 << 4) Down mode pin 4 is 0.
    //TIMER0_TAMATCHR_R = 0x00; //Match Register
    //int32_t ctime = TIMER0_TAR_R; // Current Time
    /*120MHz clock is set: tick is 120000000 ticks per sec */
    TIMER0_TAILR_R = (0xEA60-1); // Load Value/ 1000/60000()
    TIMER0_TAPR_R = (0x02-1); //sets the prescaler/ 120/2
    
}

/*******************************************************************************************/

void configure_ports() {
    
    SYSCTL_RCGCGPIO_R |= (0x1 << 3);//Switch on clock for Port D (Bit 3 of RCGCGPIO)
    // Port A=0, B=1, C=2, D=3
    while (!(SYSCTL_PRGPIO_R & 0x0008)); // Wait for clock to stabilize for Port D (Bit 3 of PRGPIO = 0x0008) 
    GPIO_PORTD_AHB_DEN_R |= 0x02;//Digital I/O enable for pins PD0 and PD1
    // (0x1 << 0) | (0x1 << 1) = 0x03
    GPIO_PORTD_AHB_DIR_R |= 0x02; //Set Direction to OUTPUT for pins PD0 and PD1  
    GPIO_PORTD_AHB_DATA_R = 0x00;//Initialize Port D outputs to zero
}

/*******************************************************************************************/


/*******************************************************************************************/

int main(void) {

    int i = 0; // loop counter

    // Configure Timers and Ports
    configure_timers();
    configure_ports();

    while(1) {
        // toggle Port D Pin 1
        GPIO_PORTD_AHB_DATA_R |= 0x02;
        // Hardware Delay
        sleep(100);
        GPIO_PORTD_AHB_DATA_R &= ~0x02;
        sleep(100);

        printf("\nCounter value: %d", i++);    
    }
}
