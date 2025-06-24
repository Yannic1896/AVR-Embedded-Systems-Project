#include "ses_fanspeed.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define FAN_SIGNAL_PIN       PE6
#define FAN_SIGNAL_DDR       DDRE
#define FAN_SIGNAL_PORT      PORTE

volatile static uint16_t edgeCount = 0;
volatile static uint16_t currentRPM = 0;

void fanspeed_init(void) {
    // Configure INT6 (PE6) as input
    FAN_SIGNAL_DDR &= ~(1 << FAN_SIGNAL_PIN);
    FAN_SIGNAL_PORT |= (1 << FAN_SIGNAL_PIN);  // Pull-up optional

    // INT6 rising edge interrupt
    EICRB |= (1 << ISC61) | (1 << ISC60);  // Rising edge
    EIMSK |= (1 << INT6);                  // Enable INT6

    // Configure Timer1 in normal mode (overflow)
    TCCR1A = 0;
    TCCR1B |= (1 << CS12) | (1 << CS10);   // Prescaler = 1024
    TIMSK1 |= (1 << TOIE1);                // Enable Timer1 overflow interrupt

    sei();  // Enable global interrupts
}

uint16_t fanspeed_getRecent(void) {
    uint16_t rpm_copy;
    cli();                     // Disable interrupts for atomic read
    rpm_copy = currentRPM;
    sei();
    return rpm_copy;
}

// INT6 ISR: Count rising edges (2 per revolution)
ISR(INT6_vect) {
    edgeCount++;
}

// Timer1 Overflow ISR (~4.19 seconds at 16 MHz, prescaler 1024)
ISR(TIMER1_OVF_vect) {
    // RPM = (edgeCount / 2) * (60 / 4.19) ≈ edgeCount * 7.16
    // Use fixed-point: RPM = (edgeCount * 716) / 100
    currentRPM = (edgeCount * 716UL) / 100;

    edgeCount = 0;  // Reset for next interval
}
