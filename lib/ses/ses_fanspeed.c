#include <avr/io.h>
#include "ses_fan.h"
#include "ses_fanspeed.h"
#include <avr/interrupt.h>
#include "ses_led.h"
#include <stdbool.h>

//Method A: More intuitive approach to caculate fan speed

// Module-private variables
static volatile uint16_t edgeCount = 0;
static volatile bool newMeasurement = false;
static volatile uint16_t currentRpm = 0;

// Number of tachometer pulses per revolution
#define PULSES_PER_REVOLUTION 2

// Median filter configuration
#define FILTER_SIZE 5 
static volatile uint16_t speedBuffer[FILTER_SIZE];
static volatile uint8_t bufferIndex = 0;
static volatile bool bufferFilled = false;

void fanspeed_init(void) {
    // Configure INT6 (PE6) for rising edge detection
    EICRB |= (1 << ISC61) | (1 << ISC60); // Rising edge triggers interrupt
    EIMSK |= (1 << INT6); // Enable INT6

    // Configure Timer1 for 1 second measurement intervals
    TCCR1A = 0; // Normal port operation, CTC mode
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC mode, prescaler 1024

    // For 16MHz clock: 16,000,000 / 1024 = 15625 counts per second
    OCR1A = 15624; // Compare match every 1s (count from 0 to 15624)

    TIMSK1 = (1 << OCIE1A); // Enable Output Compare A Match interrupt

    // Initialize filter buffer
    for (uint8_t i = 0; i < FILTER_SIZE; i++) {
        speedBuffer[i] = 0;
    }
}

ISR(INT6_vect) {
    // Only count valid edges (optional debounce if needed)
    static uint16_t lastEdgeTime = 0;
    uint16_t currentTime = TCNT1;
    
    // Simple debounce: Only count a tachometer edge if at least 10 timer counts have passed since the last one.
    if ((currentTime - lastEdgeTime) > 10) {
        edgeCount++;
        led_yellowToggle(); 
        lastEdgeTime = currentTime;
    }
}

ISR(TIMER1_COMPA_vect) {
    // Timer1 compare match interrupt
    cli(); // Disable interrupts for atomic access
    uint16_t edges = edgeCount;
    edgeCount = 0;
    sei(); // Re-enable interrupts
    
    if (edges == 0) {
        // Fan has stopped
        currentRpm = 0;
        led_redOn(); // Turn on red LED when fan stops
    } else {
        currentRpm = (edges * 60) / PULSES_PER_REVOLUTION;
        led_redOff(); // Turn off red LED when fan is running
    }
    
    // Store the measurement in the ring buffer
    speedBuffer[bufferIndex] = currentRpm;
    bufferIndex = (bufferIndex + 1) % FILTER_SIZE;
    if (bufferIndex == 0) {
        bufferFilled = true;
    }
    
    newMeasurement = true;
}

static uint16_t calculateMedian(uint16_t* values, uint8_t size) {
    uint16_t temp[FILTER_SIZE]; // Temporary array for sorting
    
    // Copy to temporary array
    for (uint8_t i = 0; i < size; i++) {
        temp[i] = values[i];
    }
    
    // Simple bubble sort
    for (uint8_t i = 0; i < size - 1; i++) {
        for (uint8_t j = 0; j < size - i - 1; j++) {
            if (temp[j] > temp[j + 1]) {
                uint16_t swap = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = swap;
            }
        }
    }
    
    // Return the middle element
    return temp[size / 2];
}

uint16_t fanspeed_getRecent(void) {
    uint16_t rpm;
    
    cli(); // Disable interrupts for atomic access
    rpm = currentRpm;
    newMeasurement = false;
    sei(); // Re-enable interrupts
    
    return rpm;
}

uint16_t fanspeed_getFiltered(void) {
    uint16_t median;
    uint16_t tempBuffer[FILTER_SIZE];
    uint8_t validSamples;
    
    cli(); // Disable interrupts for atomic access
    
    // Determine how many valid samples we have
    validSamples = bufferFilled ? FILTER_SIZE : bufferIndex;
    
    // Copy the buffer to temporary storage
    for (uint8_t i = 0; i < validSamples; i++) {
        tempBuffer[i] = speedBuffer[i];
    }
    
    sei(); // Re-enable interrupts
    
    if (validSamples == 0) {
        return 0;
    }
    
    median = calculateMedian(tempBuffer, validSamples);
    return median;
}