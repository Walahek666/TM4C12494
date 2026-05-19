/** Blinker Project for TM4C1294ncpdt
 *
 * File:    main.c
 * Author: Bhathiya Ranasinghe
 * Date: 19.05.2026
 * Version: 1
 *
 * Blinks the onboard LEDs using a busy-wait loop as delay
 */

#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************************/

void wait(int ticks) {
    for (int i = 0; i < ticks; i++);   
}

/*******************************************************************************************/

void configure_led() {
    SYSCTL_RCGCGPIO_R       |= ((0x1 << 12) | (0x1 << 5));  // switch on clock for Port N and Port F
    while (!(SYSCTL_PRGPIO_R & 0x1020));      // wait for clock to stabilize for Both Ports
    GPIO_PORTN_DEN_R        |= ((0x1 << 1) | (0x1 << 0));   // digital I/O enable pin PN1 aand PN0
    GPIO_PORTF_AHB_DEN_R    |= ((0x1 << 4) | (0x1 << 0)); //digital I/o enable pin PF0 and PF4
    GPIO_PORTN_DIR_R        |= 0x03;         // set PortN 2 Output
    GPIO_PORTF_AHB_DIR_R    |= (0x01 | 0x10); // Set Port F 2 Outputs
    GPIO_PORTN_DATA_R        = 0x00;         // set PortN Output to zero
    GPIO_PORTF_AHB_DATA_R    = 0x00;  //Set PortF Output to zero
}

/*******************************************************************************************/


/*******************************************************************************************/

int main(void) {

    int i = 0; // loop counter

    // Configure LED 1
    configure_led();

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
            
    
        // software delay
        wait(100000);
        
        printf("\nCounter value: %d", i++);    
    }
}
