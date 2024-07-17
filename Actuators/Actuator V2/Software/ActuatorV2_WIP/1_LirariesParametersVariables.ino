/*
 *      LIBRARIES
 */
#include "RPi_Pico_TimerInterrupt.h"
#include "RP2040_PWM.h"
#include <Wire.h>

/*
 *      PINS
 */
#define PIN_MOTOR_PWM_1     2
#define PIN_DRV_DECAY       3
#define PIN_MOTOR_PWM_2     4
#define PIN_DRV_SLEEP       6
#define PIN_DRV_RESET       7
#define PIN_LED_HEARTBEAT   8
#define PIN_LED_FAULT       11
#define PIN_LED_ENABLE      12
#define PIN_LED_MODE        13
#define PIN_PICO_LED        25
#define PIN_MOTOR_CURR      26
#define PIN_INA125          27
#define PIN_INA826          28

/* 
 *      PARAMETERS 
 */
#define SERIAL_BAUDRATE     115200            // [9600/115200]  - Sets the baudrate of the Serial connection
#define SERIAL_DEBUG        false              // [true / false] - Enables / Disables the Serial debug output
#define STARTSEQ_WAIT       300               // [ms]           - Determines the speed of the LED Start Sequence
#define PERIOD_TIMER_LED    100000            // [us]           - Number of microseconds a period of the timer lasts
#define PERIOD_TIMER_COM    1000              // [us]           - Number of microseconds a period of the timer lasts
#define PERIOD_TIMER_CTRL   500               // [us]           - Number of microseconds a period of the timer lasts
#define PERIOD_TIMER_SEN    500               // [us]           - Number of microseconds a period of the timer lasts
#define LEN                 16                // [b]            - Number of bits 
#define PWM_FREQUENCY       5000              // [Hz]           - PWM Frequency in Hertz
#define DATA_STREAM_LEN     12                // [#]            - Number of different data points collected by the actuator
#define INA826_K            2.2629            // [#]            - K from the calibration procedure for the INA826
#define INA826_N            1.33955           // [#]            - N from the calibration procedure for the INA826
#define ALGORITHM           2                 // [#]            - Number of the algorithm selected for the control of the actuator

// Controller Parameters & Variables
// Parameters
#define MIN_REFERENCE       0.00              // [g]            - Minimum reference that can be sent to the controller
#define MAX_REFERENCE       5000.00           // [g]            - Maximum reference that can be sent to the controller
#define MIN_OUTPUT          -100.00           // [%]            - Minimum duty cycle as the output of the controller
#define MAX_OUTPUT          100.00            // [%]            - Maximum duty cycle as the output of the controller
#define KP                  0.08              // [#]            - Kp value for the PI controller
#define KI                  0.00002           // [#]            - Ki value for the PI controller
#define ACC_MAX             1000


// Variables
volatile float reference    = 0.00;           // [g]            - Reference for the PI controller
volatile float acc          = 0.00;           // [g]            - Accumulator for the PI controller
volatile float e            = 0.00;           // [g]            - Error for the PI controller
volatile float pi_duty      = 0.00;           // [%]            - Calculated duty cycle as the output of the PI controller
volatile int   pi_dir       = 1;              // [1/2]          - Motor spinning direction based on the output of the PI controller

#define KP_BASE             0.0009              // [#]            - Base value for the KP2
#define DIR_UP              2.0               // [#]            - Direction factor for tensioning the string
#define DIR_DOWN            0.8               // [#]            - Direction factor for tensioning the 
#define FF                  0.00002            // [#]            - Force factor correction parameter
#define KD_BASE             0.9              // [#]            - Base value for the KD2
#define DF                  1.2               // [#]            - Derivative control factor for adjusting the variable part of the KP2    
#define KPROX               10                 // [#]            - Proximity correction factor for the PD controller
#define PIR                 150               // [g]            - Range for the PI controller
#define PDR                 1000              // [g]            - Range for the PD controller

// Variables for algorithm 2
volatile float last_e       = 0.00;           // [g]            - Error in the last loop iteration for the controller
volatile float KP2          = KP_BASE;        // [#]            - Kp value for the controller of the Algorithm 2
volatile float KD2          = KD_BASE;        // [#]            - Kd value for the controller of the Algorithm 2
volatile bool  last_pi      = false;          // [true/false]   - Flag for figuring out if the last controller was PI or not
volatile float KDIR2        = 0.00;           // [#]            - Direction factor used in the second algorithm

/*
 *      VARIABLES 
 */
bool error                  = false;          // Flag that tells us if we have an erro in our system
int led_skip                = 0;              // Flag for skipping turning on the built in LED
int led_state               = 1;              // Built in LED status
volatile bool new_message   = false;          // Flag for checking if there are any new messages
volatile bool new_command   = false;          // Flag that indicates if there is a new command present that needs to be processed
String serial_msg           = "";             // String for holding the Serial message
volatile byte i2c_msg[LEN]  = {};             // Variable (array) for holding the byte array which is the I2C message
volatile bool driver_enable = false;          // Flag for holding the information if the driver should be enabled or not
int decay_mode              = 0;              // Variable for holding the current decay mode, 1 - Fast Decay, 0 - Slow Decay
int CONTROLLER_MODE         = 1;              // Variable for configuring in what mode the controller is, 1 - Manual, 2 - Load Cell PI, 3 - IMU PI
int mode_led_skip           = 0;              // Flag for skipping turning on the Mode LED
int mode_led_state          = 1;              // Mode LED status
volatile int current_dir    = 1;              // The current direction in which the motor is spinning
volatile int current_duty   = 0;              // The current desired duty cycle of the PWM
bool serial_data_stream     = false;          // Flag that controls the serial data stream
volatile int motor_current  = 0;              // Variable for keeping the raw data from the ADC connected to the current measuring of the DRV8842
volatile int ina125         = 0;              // Variable for keeping the raw data from the ADC connected to the INA125 amplifier
volatile int ina826         = 0;              // Variable for keeping the raw data from the ADC connector to the INA826 amplifier
volatile float ina826g      = 0;              // Variable for keeping the calculated weight from the INA826
volatile int data_flag      = 0;

/*
 *  Data stream variable - array of numbers
 *  0  - Actuator ID
 *  1  - Current Reading
 *  2  - INA826 Reading
 *  3  - INA125 Reading
 *  4  - PWM Reading 1     - TODO
 *  5  - PWM Reading 2     - TODO
 *  6  - Encoder A         - TODO
 *  7  - Encoder B         - TODO
 *  8  - Actuator Mode     - TODO
 *  9  - Driver Fault      - TODO
 *  10 - Driver Enable     - TODO
 *  11 - Actuator Error    - TODO
 */
volatile int data_stream[DATA_STREAM_LEN] = {};

/*
 *      TIMERS 
 */
RPI_PICO_Timer              TimerLED(0);      // Timer for the heartbeat LED                          - Frequency set in setup() - Default 10 Hz
RPI_PICO_Timer              TimerCOM(1);      // Timer for setting the communication rate             - Frequency set in setup() - Default 1 kHz
RPI_PICO_Timer              TimerCTRL(2);     // Timer for controlling the motor, both manual and PI  - Frequency set in setup() - Default 2 kHz
RPI_PICO_Timer              TimerSEN(3);      // Timer for sensor data acqusition                     - Frequency set in setup() - Default 2 kHz

/*
 *      PWM
 */
RP2040_PWM*                 PWM_Instance[2];  // PWM for the Pico
