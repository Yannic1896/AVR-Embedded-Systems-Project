# AVR Embedded Systems Lab
This repository contains comprehensive lab assignments for the **Software for Embedded Systems (SES)** course, implementing advanced embedded programming concepts on AVR ATmega32U4 microcontroller.

## 📋 Project Overview

C programming on ATmega32U4 demonstrating core embedded systems concepts:

**Key Concepts Implemented:**
- **ADC interfacing** - 10-bit analog sensor reading (potentiometer, light, temperature)
- **Interrupt-driven I/O** - External interrupts for button handling with callback patterns
- **Hardware timers** - Precision timing with Timer0/Timer1 in CTC and PWM modes
- **Cooperative scheduling** - Task queue with millisecond-precision timer-based dispatch
- **Preemptive multitasking** - Context-switching scheduler with round-robin algorithm
- **PWM control** - 8-bit Fast PWM for motor speed with tachometer feedback
- **Finite state machines** - Event-driven hierarchical FSM for complex UI logic

**GPIO & Peripheral Usage:**
- **Port F (GPIO)**: LEDs (red/PF5, green/PF4, yellow/PF6)
- **Port D (Interrupts)**: Push button (INT2/PD2), Rotary encoder (INT3/PD3)
- **ADC channels**: ADC0 (light sensor), ADC6 (potentiometer), ADC7 (temperature)
- **Timer0**: 1ms system tick for scheduler and timekeeping
- **Timer1**: Fast PWM output (OC1A), Input capture (ICP1) for tachometer
- **USART1**: USB CDC serial debugging

## 🧠 Core Concepts

**Real-Time**: Interrupt latency (~4-6 cycles), ISR design (short, non-blocking), jitter, WCET  
**Scheduling**: Cooperative (low overhead, voluntary yield) vs Preemptive (forced switching, higher overhead)  
**Design Patterns**: Callback, State, Observer, Singleton

---

## 🛠️ Development Environment

**Hardware**: ATmega32U4 (8-bit AVR, 16MHz, 2.5KB RAM, 32KB flash), SES board  
**Toolchain**: avr-gcc, avrdude, PlatformIO  
**Stack**: Application → SES HAL → AVR libc → Hardware registers

## 📚 SES Hardware Abstraction Library (`lib/ses/`)

### Device Drivers
- **`ses_led`**: GPIO-based LED control (red, green, yellow)
- **`ses_button`**: Debounced push button and rotary encoder with interrupt callbacks
- **`ses_display`**: LCD/7-segment display with cursor positioning
- **`ses_timer`**: Hardware Timer0 abstraction with 1ms periodic callbacks
- **`ses_adc`**: 10-bit ADC for analog sensors (light, temperature, potentiometer)
- **`ses_fan`**: 8-bit PWM motor control with tachometer feedback
- **`ses_fanspeed`**: Fan RPM measurement with moving average filter
- **`ses_usbserial`**: CDC-ACM USB serial for debugging

### Framework Components
- **`ses_scheduler`**: Cooperative task scheduler with millisecond-precision timing
- **`ses_fsm`**: Hierarchical state machine framework with event dispatching

## 🎯 Task Documentation

### Task 2: ADC Sensor Interfacing
10-bit successive approximation ADC for analog sensors (potentiometer, light, temperature).

**Key Concepts**: Successive approximation, reference voltage, analog multiplexing, polling

---

### Task 3: Interrupt-Driven Buttons
External interrupts (INT2/INT3) with callback functions for asynchronous button handling.

**Key Concepts**: ISR, interrupt vectors, edge triggering, function pointers, interrupt latency

---

### Task 3: Hardware Timer
Timer0 in CTC mode generates 1ms periodic interrupts for precise timing.

**Key Concepts**: CTC mode, prescaler, compare match, jitter, ISR timing

**Example**:
```c
volatile uint16_t counter_ms = 0;
void timer_callback(void) {
    if (++counter_ms >= 1000) {
        led_yellowToggle();
        counter_ms = 0;
    }
}
timer0_setCallback(timer_callback);
timer0_start();
```

---

### Task 4: Cooperative Scheduler
Non-preemptive task scheduler with linked-list queue. Timer ISR decrements counters, main loop executes flagged tasks.

**Key Concepts**: Cooperative multitasking, task descriptors, periodic/one-shot tasks, atomic operations

**Example**:
```c
task_descriptor_t blinkTask = {
    .task = &blink_red,
    .param = NULL,
    .expire = 1000,  // Start after 1s
    .period = 1000   // Repeat every 1s
};
scheduler_add(&blinkTask);
scheduler_run();
```

---

### Task 4: Preemptive Multitasking
Context-switching scheduler with round-robin algorithm. Timer ISR saves all registers to task stack, switches to next task.

**Key Concepts**: Preemptive scheduling, TCB, per-task stacks (128 bytes), context switch overhead

---

### Task 5: PWM Fan Control
Closed-loop motor control with Timer1 Fast PWM (31kHz, 8-bit) and tachometer feedback via input capture. Potentiometer controls speed, 5-sample moving average filter smooths RPM readings.

**Key Concepts**: Fast PWM, input capture, moving average filter, closed-loop control

**Example**:
```c
// Timer1: 8-bit Fast PWM, ~31kHz
TCCR1A = (1<<WGM10) | (1<<COM1A1);  // Fast PWM, clear on match
TCCR1B = (1<<WGM12) | (1<<CS10);    // No prescaler
OCR1A = 128;  // 50% duty cycle

// Read potentiometer and set fan speed
uint16_t potValue = adc_read(ADC_POTI_CH);
uint8_t duty = potValue >> 2;  // Scale 10-bit to 8-bit
fan_setDutyCycle(duty);
```

---

### Task 6: FSM Alarm Clock
Event-driven state machine handling button events (PUSHBUTTON_PRESSED, ROTARYBUTTON_PRESSED) and temporal events (TIME_MATCH, TIMEOUT). Dispatcher routes events to state functions and manages transitions.

**Key Concepts**: State pattern, event-driven programming, hierarchical FSM, temporal events

**Example**:
```c
typedef struct fsm_s {
    state_t state;          // Current state function pointer
    time_t alarmTime;
    bool isAlarmEnabled;
} fsm_t;

// Event signals
typedef enum {
    ENTRY, EXIT,
    PUSHBUTTON_PRESSED,
    ROTARYBUTTON_PRESSED,
    TIME_MATCH, TIMEOUT
} signal_t;
```

---



## 🧪 Debugging

**Serial**: `fprintf(serialout, "Debug: %u\n", value);`  
**LEDs**: Red (error/alarm), Yellow (states), Green (success)  
**Common Issues**: Missing `sei()`, busy-wait in ISR, floats in ISR, uninitialized pointers

---
