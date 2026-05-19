/** Blinker Project for TM4C1294ncpdt
 *
 * File:    main.c
 * Author:
 * Date:
 * Version:
 *
 * Blinks the onboard LED using a busy-wait loop as delay
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
    SYSCTL_RCGCGPIO_R       |= (0x1 << 12);  // switch on clock for Port N
    while (!(SYSCTL_PRGPIO_R & 0x1000));      // wait for clock to stabilize
    GPIO_PORTN_DEN_R        |= ((0x1 << 1) | (0x1 << 0));   // digital I/O enable pin PN1 aand PN0
    GPIO_PORTN_DIR_R        |= 0x03;         // set PortN 1 Output
    GPIO_PORTN_DATA_R        = 0x00;         // set PortN Output to zero
}

/*******************************************************************************************/


/*******************************************************************************************/

int main(void) {

    int i = 0; // loop counter

    // Configure LED 1
    configure_led();

    while(1) {
        // toggle LEDs
        if ((GPIO_PORTN_DATA_R & 0x02))
        { // check if led is ON
            GPIO_PORTN_DATA_R &= ~0x02; // turn LED 2 off
            GPIO_PORTN_DATA_R |= 0x01; // turn LED 1 On
        }
        else{
            GPIO_PORTN_DATA_R &= ~0x01; // turn LED 1 off
            GPIO_PORTN_DATA_R |= 0x02; //tuen LED 2 On


        }
            
    
        // software delay
        wait(100000);
        
        printf("\nCounter value: %d", i++);    
    }
}
