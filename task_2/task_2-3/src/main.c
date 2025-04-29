#include "ses_usbserial.h"
#include "ses_display.h"

int main(void){

usbserial_init();
display_init();
sei(); // Activate interrupts. Required to use USB serial communication.

while (1) {
    printf(serialout, "Your USB output goes here\n");
    display_setCursor(0,0); // Set cursor to column 0, row 0
    fprintf(displayout, "Your display output goes here");
    display_update();
}

}