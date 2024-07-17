/*
 *      LED TIMER
 */
bool HeartbeatLEDHandler(struct repeating_timer *t)
{
  (void) t;
  // Heartbeat LED
  if(error == true)
  {
    // If we're in error mode, we don't do a skip, we toggle the LED with every timer call
    if(led_state == 1)  digitalWrite(PIN_PICO_LED, HIGH);
    if(led_state == -1) digitalWrite(PIN_PICO_LED, LOW);
    if(led_state == 1)  digitalWrite(PIN_LED_HEARTBEAT, HIGH);
    if(led_state == -1) digitalWrite(PIN_LED_HEARTBEAT, LOW);
    led_state = -1 * led_state;
  }
  else
  {
    // If we're not in error mode, we need to skip every other toggle
    if(led_skip == 10)
    {
      if(led_state == 1)  digitalWrite(PIN_PICO_LED, HIGH);
      if(led_state == -1) digitalWrite(PIN_PICO_LED, LOW);
      if(led_state == 1)  digitalWrite(PIN_LED_HEARTBEAT, HIGH);
      if(led_state == -1) digitalWrite(PIN_LED_HEARTBEAT, LOW);
      led_state = -1 * led_state;
      led_skip = 0;
    }
    else
    {
      led_skip++;
    }
  }

  // Mode LED
  if(CONTROLLER_MODE == 3)
  {
    if(mode_led_state == -1)  digitalWrite(PIN_LED_MODE, HIGH);
    if(mode_led_state == 1) digitalWrite(PIN_LED_MODE, LOW);
    mode_led_state = -1 * mode_led_state;
  }
  else if (CONTROLLER_MODE == 1)
  {
    // If we're not in error mode, we need to skip every other toggle
    if(mode_led_skip >= 10)
    {
      if(mode_led_state == -1)  digitalWrite(PIN_LED_MODE, HIGH);
      if(mode_led_state == 1) digitalWrite(PIN_LED_MODE, LOW);
      mode_led_state = -1 * mode_led_state;
      mode_led_skip = 0;
    }
    else
    {
      mode_led_skip++;
    }
  }
  else if (CONTROLLER_MODE == 2)
  {
    if(mode_led_state == -1)  digitalWrite(PIN_LED_MODE, HIGH);
    if(mode_led_state == 1) digitalWrite(PIN_LED_MODE, LOW);
  }
  /*
  // Serial data stream
  if (serial_data_stream == true)
  {
    PrintDataStream();
  }*/
  
  return true;
}

/*
 *      COM TIMER 
 */
bool CommunicationHandler(struct repeating_timer *t)
{
  (void) t;
  
  // Check if there are any commands that need to be executed
  if (new_command == true)
  {
    new_command = false;
    switch(i2c_msg[0])
    {
      case 1:
      {
        // Command C1 - Enable/Disable driver
        if (i2c_msg[1] == 1)
        {
          sprintln("Received command C1 to Enable Driver.");
          sprintln("Pink LED should turn ON.");
          driver_enable = true;
        }
        else
        {
          sprintln("Received command C1 to Disable Driver.");
          sprintln("Pink LED should turn OFF");
          driver_enable = false;
        }
        UpdateDriver(driver_enable);
        break;
      }
      case 2:
      {
        // Command C2 - Change Decay Mode
        if (i2c_msg[1] == 1)
        {
          sprintln("Switching to Fast Decay mode.");
          decay_mode = 1;
        }
        else
        {
          sprintln("Switching to Slow Decay mode.");
          decay_mode = 0;
        }
        DecayMode(decay_mode);
        break;
      }
      case 3:
      {
        // Command C3 - Change Controller Mode
        // Mode 1 - Manual Control Mode
        // Mode 2 - Load Cell PI Mode
        // Mode 3 - IMU PI Mode
        if (i2c_msg[1] == 1)
        {
          sprintln("Switching to Mode 1.");
          CONTROLLER_MODE = 1;
        }
        else if (i2c_msg[1] == 2)
        {
          sprintln("Switching to Mode 2.");
          mode_led_state  = -1;
          CONTROLLER_MODE = 2;
        }
        else if (i2c_msg[1] == 3)
        {
          sprintln("Switching to Mode 3.");
          CONTROLLER_MODE = 3;
        }
        break;
      }
      case 4:
      {
        // Command C4 - Manual PWM control
        // Updating the direction
        if (i2c_msg[1] != 3)
        {
          current_dir = i2c_msg[1];
        }
        // Updating the speed
        current_duty = i2c_msg[2];
        // Sending the command to the motor driver
        sprintln("Sending new PWM command to the motor driver.");
        MotorDriver(current_dir, current_duty);
        break;
      }
      case 5:
      {
        sprintln("Received command for updating the PI controller reference");
        reference = i2c_msg[1] * 255 + i2c_msg[2];
        sprint("Requested reference: ");
        sprintln(String(reference));
        if (reference > MAX_REFERENCE)
        {
          reference = 0;
          sprintln("ERROR - Requested reference is out of range (over max), MAX_REFERENCE = ");
          sprintln(String(MAX_REFERENCE));
          Error(1);
        }
        else if (reference < MIN_REFERENCE)
        {
          reference = 0;
          sprintln("ERROR - Requested reference is out of range (under min), MIN_REFERENCE = ");
          sprintln(String(MIN_REFERENCE));
          Error(1);
        } 
        break;
      }
      case 10:
      {
        sprintln("Received command to reset all errors on the controller.");
        Error(0);
        break;
      }
      case 13:
      {
        // Command C13 - Data Stream
        sprintln("Received command for toggling the data stream.");
        if (i2c_msg[1] == 0)
        {
          // Disable the stream
          sprintln("Disabling the stream.");
          serial_data_stream = false;
        }
        else if (i2c_msg[1] == 1)
        {
          // Enable the stream
          sprintln("Enabling the stream.");
          serial_data_stream = true;
        }     
        break;
      }
    }
    // Reset the message at the very end
    ResetMsg();
  }
  
  // Checking Serial port for any new messages
  if (new_message == false)
  {
    CheckSerial();
  }
  else
  {
    new_message = false;
    sprintln(serial_msg);
    DecodeSerialMessage(serial_msg);
  }
  /*
  // Serial data stream
  if (serial_data_stream == true)
  {
    PrintDataStream();
  }
  */
  return true;
}

/*
 *      CTRL TIMER
 */
bool ControlHandler(struct repeating_timer *t)
{
  (void) t;
  // Depending on the CONTROLLER_MODE variable, we can switch between different modes for controlling the actuator motor
  // Reading the mode from the MODE LED
  // Slow Blink   - Mode 1
  // Solid Color  - Mode 2
  // Fast Blink   - Mode 3
  switch(CONTROLLER_MODE)
  {
    case 1:
    {
      // MANUAL MODE
      break;
    }
    case 2:
    {
      // LOAD CELL PI MODE
      // If the reference has been set out of range somehow, go into error state
      if (reference < MIN_REFERENCE || reference > MAX_REFERENCE)
      {
        sprintln("ERROR - Reference out of bounds!");
        sprint("Reference range is: ");
        sprint(String(MIN_REFERENCE));
        sprint(" - ");
        sprintln(String(MAX_REFERENCE));
        sprint("Reference received: ");
        sprintln(String(reference));
        Error(1);
      }
      // If the reference is exactly the same as the MIN_REFERENCE value, send duty_cycle 0%
      if (reference == MIN_REFERENCE)
      {
        // Updating the motor driver and pi_duty
        pi_duty = 0.00;
        MotorDriver(pi_dir, abs(int(pi_duty)));
        // Besides that, also reset the accumulator
        acc = 0;
      }
      else
      {
        /*
         * 
         *  THIS IS THE LOCATION OF ALL CONTROLLER ALGORITHMS
         * 
         */

        /*
         *  ALGORITHM 1
         */
        if (ALGORITHM == 1)
        {
          // First we calculate is the error e
          e = ina826g - reference;
          // Update the accumulator acc
          acc += e;
          
          // Calculate the output duty cycle
          pi_duty = KP * e + KI * acc;
          
          // Check for the boundary conditions
          if (pi_duty > MAX_OUTPUT)
          {
            // Set the pi_duty to MAX_OUTPUT and alter the acc
            pi_duty = MAX_OUTPUT;
            acc -= e; 
          }
          if (pi_duty < MIN_OUTPUT)
          {
            // Set the pi_duty to MAX_OUTPUT and alter the acc
            pi_duty = MIN_OUTPUT;
            acc -= e; 
          }
        }
        else if(ALGORITHM == 2)
        {
          // First we update the last_e with the value of e
          last_e = e;
          // We calculate is the error e
          e = ina826g - reference;
          // Checking the sign of the error
          if (e > 0)
          {
            // If e is greated than 0, it means we're releasing tension
            KDIR2 = DIR_DOWN;
          }
          else
          {
            // If e is lower than 0, it means we're applying more tension
            KDIR2 = DIR_UP;
          }

          
          if (abs(e) < PIR)
          {
            /*
             *    PI CONTROLLER
             */
            // If we're entering the PI controller from another controller, reset the acc
            if (last_pi == false) acc = 0;
            // Change the last_pi flag to true
            last_pi = true;
            
            // Calculate the new value for the KP2
            KP2 = KP_BASE;
            
            // Calculate the output duty cycle
            pi_duty = 2 * KP2 * e + KI * acc;

            // Check for the boundary conditions
            if (pi_duty > MAX_OUTPUT)
            {
              // Set the pi_duty to MAX_OUTPUT and alter the acc
              pi_duty = MAX_OUTPUT;
              acc -= e; 
            }
            if (pi_duty < MIN_OUTPUT)
            {
              // Set the pi_duty to MAX_OUTPUT and alter the acc
              pi_duty = MIN_OUTPUT;
              acc -= e; 
            }
          }
          else if (abs(e) < PDR)
          {
            /*
             *    PD CONTROLLER
             */
            // Update the last_pi flag
            last_pi = false;

            // Calculate the new value for the KP2
            KP2 = KP_BASE + KDIR2 * FF * ina826g;

            // Calculate the new value for the KD2
            KD2 = KD_BASE + DF * FF * ina826g + KPROX / abs(e);

            // Calculate the output duty cycle
            pi_duty = KP2 * e + KD2 * (e - last_e);

            // Check for the boundary conditions
            if (pi_duty > MAX_OUTPUT)
            {
              // Set the pi_duty to MAX_OUTPUT and alter the acc
              pi_duty = MAX_OUTPUT;
            }
            if (pi_duty < MIN_OUTPUT)
            {
              // Set the pi_duty to MAX_OUTPUT and alter the acc
              pi_duty = MIN_OUTPUT;
            }
          }
          else
          {
            /*
             *    P CONTROLLER
             */
            // Update the last_pi flag
            last_pi = false;

            // Calculate the new value for the KP2
            KP2 = KP_BASE + KDIR2 * FF * ina826g;

            // Calculate the output duty cycle
            pi_duty = KP2 * e;

            // Check for the boundary conditions
            if (pi_duty > MAX_OUTPUT)
            {
              // Set the pi_duty to MAX_OUTPUT and alter the acc
              pi_duty = MAX_OUTPUT;
            }
            if (pi_duty < MIN_OUTPUT)
            {
              // Set the pi_duty to MAX_OUTPUT and alter the acc
              pi_duty = MIN_OUTPUT;
            }
          }
          
        }


        /*
         *  THIS PART IS UNIVERSAL FOR ALL ALGORITHMS
         */
        // Get the direction from the sign of the PI output
        if (pi_duty >= 0) pi_dir = 2;
        if (pi_duty < 0)  pi_dir = 1;
        // At this point update the motor driver with the newly calculated duty cycle and motor direction
        MotorDriver(pi_dir, abs(int(pi_duty)));
        
      }
      break;
    }
    case 3:
    {
      // IMU PI MODE
      break;
    }
    default:
    {
      break;
    }
  }
  return true;
}

/*
 *      CTRL TIMER
 */
bool SenHandler(struct repeating_timer *t)
{
  (void) t;
  // In every loop of this timer, make sure to update the variables by reading the data from the analog pins
  motor_current   = analogRead(PIN_MOTOR_CURR);
  ina125          = analogRead(PIN_INA125);
  ina826          = analogRead(PIN_INA826);
  // Calculate the force/weigth using the ADC value and the calculated parameters from the calibration procedure
  ina826g         = INA826_K * ina826 + INA826_N;
  data_flag++;
  if (data_flag > 5 && serial_data_stream == true)
  {
    PrintDataStream();
    data_flag = 0;
  }
  return true;
}
