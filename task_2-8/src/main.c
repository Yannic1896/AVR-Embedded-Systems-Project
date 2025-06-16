#include "ses_usbserial.h"
#include "ses_display.h"
#include "ses_adc.c"
#include <util/delay.h>



int main(void) {
    usbserial_init();
    sei();

    adc_init();

    while(1){

    fprintf(serialout, "init done\n");

    uint16_t adc_value = 0;

    adc_value = adc_read(7);
    fprintf(serialout, "adc read\n");
    fprintf(serialout, "ADC value: %d\n", adc_value);

    _delay_ms(1000);
    }
    return 0;
    
}
