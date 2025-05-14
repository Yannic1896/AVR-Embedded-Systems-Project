/* INCLUDES ******************************************************************/

#include <avr/io.h>
#include "ses_led.h"

/* DEFINES & MACROS **********************************************************/

// LED wiring on SES board
#define BUTTON_ROTARY_PORT          PORTB
#define BUTTON_ROTARY_DDR           DDRB
#define BUTTON_ROTARY_BIT           5

#define BUTTON_PUSH_PORT            PORTB
#define BUTTON_PUSH_DDR           DDRB
#define BUTTON_PUSH_BIT             4


/* FUNCTION DEFINITION ******************************************************/

void button_init(void){
    DDRB &= ~((1 << BUTTON_ROTARY_BIT) | (1 << BUTTON_PUSH_BIT)); // Define Bits as input
    PORTB |= (1 << BUTTON_ROTARY_BIT) | (1 << BUTTON_PUSH_BIT);
}

 /* Get the state of the pushbutton.
 */
_Bool button_isPushButtonPressed(void){
    return !(PINB & (1 << BUTTON_PUSH_BIT));        // Return true if button pushed
}

/** 
 * Get the state of the rotary button.
 */
_Bool button_isRotaryButtonPressed(void){
    return !(PINB & (1 << BUTTON_ROTARY_BIT));      // Return true if button pushed
}
