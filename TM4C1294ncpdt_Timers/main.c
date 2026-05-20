/** GPIO Timers Project for TM4C1294ncpdt
 *
 * File:    main.c
 * Author: Bhathiya Ranasinghe
 * Date: 20.05.2026
 * Version: 1
 *
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
TIMER0_CTL_R |= 0x0001; //Sub Timer A in Timer 0 (Start / Stop Timer)
while (!(TIMER0_RIS_R & 0x01)); //Timer A Time out 
//Raw Interrupt(TATORIS), Bit #4 Timer A Match RAW Interrupt (TAMRIS)
//Hardware set corresponding bits to 1 to Flag.
TIMER0_ICR_R |= 0x01; //Iterrupt Clear Softwaare Set 1 to Clear


}

//Configure Timers
void configure_timers() {
    SYSCTL_RCGCTIMER_R |= (0x1 << 0); //activating Timer module 0
    while (!(SYSCTL_PRTIMER_R & 0x01));      // wait for clock to stabilize for the Timer
    (*((uint32_t *)0x4003000C)) = 0x000;   // disable Timer0 A
    TIMER0_CFG_R = 0x0004; //16bit Mode in Timer 0, 0x0 32 bit mode, 0x1 RTC
    TIMER0_TAMR_R |= 0x11; //Pin 5 Match enabled for Interrupts
    // Timer AB Mode 0x1 One shot ox2 Periodic Timer
    //pin4 UpDown Mode  | (0x1 << 4), Pin5 is Match Enable
    //TIMER0_TAMATCHR_R = 0x00; //Match Register
    //int32_t ctime = TIMER0_TAR_R; // Current Time
    TIMER0_TAILR_R = 65041-1; // Load Value
    TIMER0_TAPR_R = 123-1; //sets the prescaler to 100
    
}

/*******************************************************************************************/

void configure_ports() {
    SYSCTL_RCGCGPIO_R       |= ((0x1 << 12) | (0x1 << 5));      // switch on clock for Port N and Port F
    while (!(SYSCTL_PRGPIO_R & 0x1020));      // wait for clock to stabilize for Both Ports
    GPIO_PORTN_DEN_R        |= ((0x1 << 1) | (0x1 << 0));   // digital I/O enable pin PN1 aand PN0
    GPIO_PORTF_AHB_DEN_R    |= ((0x1 << 4) | (0x1 << 0));   //digital I/o enable pin PF0 and PF4
    GPIO_PORTN_DIR_R        |= 0x03;         // set PortN 2 Output
    GPIO_PORTF_AHB_DIR_R    |= (0x01 | 0x10);       // Set Port F 2 Outputs
    GPIO_PORTN_DATA_R        = 0x00;         // set PortN Output to zero
    GPIO_PORTF_AHB_DATA_R    = 0x00;        //Set PortF Output to zero
}

/*******************************************************************************************/


/*******************************************************************************************/

int main(void) {

    int i = 0; // loop counter

    // Configure Timers and Ports
    configure_timers();
    configure_ports();

    while(1) {
        // toggle 4 LEDs
        if ((GPIO_PORTN_DATA_R & 0x02))
        { // check if One led is ON for the logic

            GPIO_PORTN_DATA_R &= ~0x02; // turn LED 1 off
            GPIO_PORTN_DATA_R |= 0x01; // turn LED 2 On
            GPIO_PORTF_AHB_DATA_R  |= 0x01; 
            GPIO_PORTF_AHB_DATA_R  &= ~0x10;
        }
        else{
            GPIO_PORTN_DATA_R &= ~0x01; // turn LED 2 off
            GPIO_PORTN_DATA_R |= 0x02; //turn LED 1 On
            GPIO_PORTF_AHB_DATA_R  &= ~0x01; 
            GPIO_PORTF_AHB_DATA_R  |= 0x10; 


        }
            
    
        // Hardware Delay
        sleep(100000);
        
        printf("\nCounter value: %d", i++);    
    }
}
