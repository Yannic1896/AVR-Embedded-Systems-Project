#include "ses_adc.h"
#include <avr/io.h>


#define ADC_TEMP_RAW_LOW       // ADC reading at 10.0°C
#define ADC_TEMP_RAW_HIGH      // ADC reading at 30.0°C
#define ADC_TEMP_LOW       100    // 10.0°C → in 0.1°C units
#define ADC_TEMP_HIGH      300    // 30.0°C → in 0.1°C units

void adc_init(void) {
    
    /*
    Potentiometer Pin6 PortF
    Temperature Sensor Pin 7 PortF
    Light Sensor Pin0 PortF
    */

    /* Set ADC pins as input and disable pull-up resistors */
    DDRF &= ~((1 << PF0) | (1 << PF6) | (1 << PF7));     // Clear bits to set them as input
    PORTF &= ~((1 << PF0) | (1 << PF6) | (1 << PF7));    // Clear bits to disable pullups

    /* Disable power reduction for ADC */
    PRR0 &= ~(1 << PRADC);          // clear bit in register PRR0

    /* Configure ADMUX */
    ADMUX = (0 << REFS1) | (0 << REFS0);  // Use voltage on AREF Pin as external reference 
    ADMUX &= ~(1 << ADLAR);              // Right-adjusted result

    /* Configure ADCSRA */
    ADCSRA = (1 << ADEN);                // Enable ADC hardware clears all other bits
    ADCSRA |= ADC_PRESCALE;              // Set prescaler (111) -> 16 MHz / 128 = 125 kHz

}

uint16_t adc_read(uint8_t adc_channel) {
    // Validate the input channel
    if (adc_channel != ADC_LIGHT_CH &&
        adc_channel != ADC_POTI_CH &&
        adc_channel != ADC_TEMP_CH) {
        return ADC_INVALID_CHANNEL;
    }

    /* Set the ADC channel in ADMUX */
    ADMUX = (ADMUX & 0xF0) | (adc_channel & 0x0F); // Keep REFS, clear MUX bits, set channel
    ADMUX &= ~(1 << ADLAR);  // Ensure right-adjusted result

    /* Start conversion */
    ADCSRA |= (1 << ADSC);

    /* Wait for conversion to complete */
    while (ADCSRA & (1 << ADSC));

    /* Read ADC result: read ADCL first, then ADCH */
    uint16_t result = ADC;

    return result;
}


int16_t adc_getTemperature(void) {
    int32_t adc = adc_read(ADC_TEMP_CH);

    // Linear interpolation in fixed-point arithmetic (scaled by 10)
    /*return (int16_t)(((adc - ADC_TEMP_RAW_LOW) * (ADC_TEMP_HIGH - ADC_TEMP_LOW))
            / (ADC_TEMP_RAW_HIGH - ADC_TEMP_RAW_LOW)) + ADC_TEMP_LOW;
            */
}