#include <avr/io.h>
#include <stdbool.h>
#include "ses_button.h"
#include <stddef.h>

// Button wiring on SES board
#define BUTTON_ROTARY_PIN   PINB
#define BUTTON_ROTARY_PORT  PORTB
#define BUTTON_ROTARY_DDR   DDRB
#define BUTTON_ROTARY_BIT   5

#define BUTTON_PUSH_PIN     PINB
#define BUTTON_PUSH_PORT    PORTB
#define BUTTON_PUSH_DDR     DDRB
#define BUTTON_PUSH_BIT     4

// Function pointers for button callbacks
static pButtonCallback rotaryButtonCallback = NULL;
static pButtonCallback pushButtonCallback = NULL;

void button_init(void) {
    // Configure rotary button pin as input
    BUTTON_ROTARY_DDR &= ~(1 << BUTTON_ROTARY_BIT);
    // Activate internal pull-up resistor for rotary button
    BUTTON_ROTARY_PORT |= (1 << BUTTON_ROTARY_BIT);

    // Configure push button pin as input
    BUTTON_PUSH_DDR &= ~(1 << BUTTON_PUSH_BIT);
    // Activate internal pull-up resistor for push button
    BUTTON_PUSH_PORT |= (1 << BUTTON_PUSH_BIT);

    // Enable pin change interrupt for PORTB in PCICR
    PCICR |= (1 << PCIE0); // PCIE0 corresponds to PORTB

    // Enable pin change interrupt for the specific pins in PCMSK0
    PCMSK0 |= (1 << BUTTON_ROTARY_BIT) | (1 << BUTTON_PUSH_BIT);
}

bool button_isPushButtonPressed(void) {
    // Check if push button is pressed (logic low)
    return !(BUTTON_PUSH_PIN & (1 << BUTTON_PUSH_BIT));
}

bool button_isRotaryButtonPressed(void) {
    // Check if rotary button is pressed (logic low)
    return !(BUTTON_ROTARY_PIN & (1 << BUTTON_ROTARY_BIT));
}

void button_setRotaryButtonCallback(pButtonCallback callback) {
    rotaryButtonCallback = callback; // Store the callback function pointer
}

void button_setPushButtonCallback(pButtonCallback callback) {
    pushButtonCallback = callback; // Store the callback function pointer
}

ISR(PCINT0_vect) {
    // Check if the rotary button interrupt is unmasked and callback is set
    if ((PCMSK0 & (1 << BUTTON_ROTARY_BIT)) && rotaryButtonCallback != NULL) {
        if (button_isRotaryButtonPressed()&& pushButtonCallback != NULL) {
            rotaryButtonCallback(); // Execute rotary button callback
        }
    }

    // Check if the push button interrupt is unmasked and callback is set
    if ((PCMSK0 & (1 << BUTTON_PUSH_BIT)) && pushButtonCallback != NULL) {
        if (button_isPushButtonPressed()&& rotaryButtonCallback != NULL) {
            pushButtonCallback(); // Execute push button callback
        }
    }
}