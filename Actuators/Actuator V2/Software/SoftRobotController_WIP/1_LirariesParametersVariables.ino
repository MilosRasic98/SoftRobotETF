/*
 *      LIBRARIES
 */
#include "RPi_Pico_TimerInterrupt.h"
#include <Wire.h>

/*
 *      PINS
 */
#define PIN_SDA 20
#define PIN_SCL 21

/* 
 *      PARAMETERS 
 */
#define SERIAL_BAUDRATE     115200            // [9600/115200]  - Sets the baudrate of the Serial connection
#define SERIAL_DEBUG        true              // [true / false] - Enables / Disables the Serial debug output
#define PERIOD_TIMER_COM    1000              // [us]           - Number of microseconds a period of the timer lasts
#define PERIOD_TIMER_CTRL   500               // [us]           - Number of microseconds a period of the timer lasts
#define LEN                 16                // [b]            - Number of bits 
#define ACTUATOR1           0x11              // [#]            - I2C Address for Actuator 1
#define ACTUATOR2           0x22              // [#]            - I2C Address for Actuator 2
#define ACTUATOR3           0x33              // [#]            - I2C Address for Actuator 3
#define ACTUATOR4           0x44              // [#]            - I2C Address for Actuator 4


/*
 *      VARIABLES 
 */
volatile bool new_message   = false;          // Flag for checking if there are any new messages
volatile bool new_command   = false;          // Flag that indicates if there is a new command present that needs to be processed
String serial_msg           = "";             // String for holding the Serial message
volatile byte i2c_msg[LEN]  = {};             // Variable (array) for holding the byte array which is the I2C message
volatile bool driver_enable = false;          // Flag for holding the information if the driver should be enabled or not
int CONTROLLER_MODE         = 1;              // Variable for configuring in what mode the controller is, 1 - Manual, 2 - Load Cell PI, 3 - IMU PI

/*
 *      TIMERS 
 */
RPI_PICO_Timer              TimerCOM(0);      // Timer for setting the communication rate             - Frequency set in setup() - Default 1 kHz
RPI_PICO_Timer              TimerCTRL(1);     // Timer for controlling the motor, both manual and PI  - Frequency set in setup() - Default 2 kHz
