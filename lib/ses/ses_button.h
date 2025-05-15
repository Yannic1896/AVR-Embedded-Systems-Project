#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_

/* INCLUDES ******************************************************************/

#include <stdbool.h>

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes rotary encoder button and pushbutton
 */
void button_init(void);

/** 
 * Get the state of the pushbutton.
 */
bool button_isPushButtonPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryButtonPressed(void);

typedef void (*pButtonCallback)(void);

//Callback functions
void button_setRotaryButtonCallback(pButtonCallback callback);
void button_setPushButtonCallback(pButtonCallback callback);

#endif /* SES_BUTTON_H_ */
