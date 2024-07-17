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
        break;
      }
      case 2:
      {
        // Command C2 - Change Decay Mode
        if (i2c_msg[1] == 1)
        {
          sprintln("Switching to Fast Decay mode.");
          //decay_mode = 1;
        }
        else
        {
          sprintln("Switching to Slow Decay mode.");
          //decay_mode = 0;
        }
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
          //mode_led_state  = -1;
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
        /*if (i2c_msg[1] != 3)
        {
          current_dir = i2c_msg[1];
        }
        // Updating the speed
        current_duty = i2c_msg[2];
        // Sending the command to the motor driver*/
        sprintln("Sending new PWM command to the motor driver.");
        break;
      }
      case 5:
      {/*
        sprintln("Received command for updating the PI controller reference");
        //reference = i2c_msg[1] * 255 + i2c_msg[2];
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
        } */
        break;
      }
      case 10:
      {
        sprintln("Received command to reset all errors on the controller.");
        //Error(0);
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
          //serial_data_stream = false;
        }
        else if (i2c_msg[1] == 1)
        {
          // Enable the stream
          sprintln("Enabling the stream.");
          //serial_data_stream = true;
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
  return true;
}

/*
 *      CTRL TIMER
 */
bool ControlHandler(struct repeating_timer *t)
{
  (void) t;
  
  return true;
}
