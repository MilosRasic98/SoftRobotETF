// Function for decoding the Serial message coming from the user
void DecodeSerialMessage(String msg)
{
  // First we check the type of command that is being sent to the Soft Robot
  // For a full list of commands please check: https://docs.google.com/spreadsheets/d/1j4l9SDbZnH_yYO-BC3xJZ4RkYJjM1L0_M2_zAHDFm0k/edit?usp=sharing

  // Command 0 [C0] - Stop all
  // Command format:      C0
  // Command example:     C0
  // Command description: When driver receives this command, it stops all motors and changed the LED-s from GREEN to RED
  // Command in I2C:      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
  // This is the only command that will be executed immediatelly here
  if (msg.startsWith("C0") == true)
  {
    sprintln("Received command C0 to do an emergency stop!");
    ResetMsg();
    return;
  }

  // Command 1 [C1] - Enable / Disable Driver
  // Command format:      C1 1/0
  // Command example:     C1 1
  // Command description: Enable or Disable the DRV8842 motor driver IC
  // Command in I2C:      {1, X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
  if (msg.startsWith("C1 ") == true)
  {
    sprintln("Received command C1.");
    ResetMsg();
    // We need to check whether we need to enable or disable the driver, which depends on msg[3]
    if (msg.startsWith("C1 1") == true)
    {
      // This is the case where we enable the driver
      i2c_msg[0]  = 1;
      i2c_msg[1]  = 1;
      new_command = true;
      return;
    }
    else if (msg.startsWith("C1 0") == true)
    {
      // This is the case where we disable the driver
      i2c_msg[0]  = 1;
      i2c_msg[1]  = 0;
      new_command = true;
      return;
    }
    // If we're here, that means that the command was sent properly
    sprintln("Command C1 was sent with a bad argument, double check the code.");
    return;
  }

  // Command 2 [C2] - Decay Mode
  // Command format:      C2 1/0
  // Command example:     C2 1
  // Command description: When driver receives this command, it changes the decay mode
  // Command in I2C:      {2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
  if (msg.startsWith("C2 ") == true)
  {
    sprintln("Received command C2.");
    ResetMsg();
    // We need to check whether we want to switch either to Fast Decay or Slow Decay mode
    if (msg.startsWith("C2 1") == true)
    {
      // Switching to fast decay mode
      i2c_msg[0]  = 2;
      i2c_msg[1]  = 1;
      new_command = true;
      return;
    }
    else if (msg.startsWith("C2 0") == true)
    {
      // Switching to slow decay mode
      i2c_msg[0]  = 2;
      i2c_msg[1]  = 0;
      new_command = true;
      return;
    }
    // If we're here, that means that the command was sent properly
    sprintln("Command C2 was sent with a bad argument, double check the code.");
    return;
  }
  

  // Command 3 [C3] - Change Mode
  // Command format:      C3 1/0
  // Command example:     C3 1
  // Command description: When driver receives this command, it changed the mode of the controller
  // Command in I2C:      {3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
  if (msg.startsWith("C3 ") == true)
  {
    sprintln("Received command C3.");
    ResetMsg();
    // We need to check to what mode we want to change
    if (msg.startsWith("C3 1") == true)
    {
      // Switching to mode 1
      i2c_msg[0]  = 3;
      i2c_msg[1]  = 1;
      new_command = true;
      return;
    }
    else if (msg.startsWith("C3 2") == true)
    {
      // Switching to mode 2
      i2c_msg[0]  = 3;
      i2c_msg[1]  = 2;
      new_command = true;
      return;
    }
    else if (msg.startsWith("C3 3") == true)
    {
      // Switching to mode 3
      i2c_msg[0]  = 3;
      i2c_msg[1]  = 3;
      new_command = true;
      return;
    }
    // If we're here, that means that the command was sent properly
    sprintln("Command C3 was sent with a bad argument, double check the code.");
    return;
  }

  // Command 4 [C4] - Manual Drive
  // Command format:      C4 DX SY 
  // Command example:     C4 D1 S50
  // Command description: When driver receives this command, if it's in manual mode, it will set the PWM to the desired value
  // Parameter limits:    The direction can be [1, 2, 3] and the speed can be [0...100]
  // Command in I2C:      {4, 1, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
  if (msg.startsWith("C4 ") == true)
  {
    sprintln("Received command C4.");
    ResetMsg();
    // We first need to check whether the actuator is in the correct mode
    if (CONTROLLER_MODE != 1)
    {
      // If we're here it means we're not in the suitable mode to handle this command, abort command parsing
      sprintln("Actuator not in the correct mode to receive manual command, it should be in mode 1.");
      sprint("Current mode: ");
      sprintln(String(CONTROLLER_MODE));
      return;
    }
    // Warn the user if the motor driver hasn't been enabled prior to sending the new command
    if (driver_enable == false)
    {
      sprintln("Warning - received manual command - driver not enabled yet!");
    }
    // We need to first parse the motor direction and speed from the command
    int new_dir     = 0;
    int new_speed   = 0;
    int dir_pos, speed_pos;
    // Direction data position within the command
    dir_pos = msg.indexOf("D");
    if (dir_pos == -1)
    {
      sprintln("Command C4 was sent with a bad argument for direction, double check the code - No direction in the command.");
      return;
    }
    // Parsing of the actual data
    new_dir = msg.substring(dir_pos + 1, dir_pos + 2).toInt();
    if (new_dir < 1 || new_dir > 3)
    {
      sprintln("Command C4 was sent with a bad argument for direction, double check the code - Direction out of range.");
      return;
    }
    // Speed data position within the command
    speed_pos = msg.indexOf("S");
    if (speed_pos == -1)
    {
      sprintln("Command C4 was sent with a bad argument for speed, double check the code - No speed in the command.");
      return;
      }
    // Parsing of the actual data
    new_speed = msg.substring(speed_pos + 1).toInt();
    if (new_speed < 0 || new_speed > 100)
    {
      sprintln("Command C4 was sent with a bad argument for speed, double check the code - Speed out of range.");
      return;
    }  
    // Parsed information
    sprint("New direction: ");
    sprintln(String(new_dir));
    sprint("New speed: ");
    sprintln(String(new_speed));
    // Put all of that info into the i2c command message
    i2c_msg[0] = 4;
    i2c_msg[1] = new_dir;
    i2c_msg[2] = new_speed;
    new_command = true;
    return;
  }

  // Command 5 - Set Reference for PI controller
  // Command format:      C5 X
  // Command example:     C5 1250
  // Command description: Update the reference for the PI controller
  // Command in I2C:      {5, X1, X2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
  if (msg.startsWith("C5 ") == true)
  {
    sprintln("Received command C5 to update reference");
    new_command = true;
    ResetMsg();
    i2c_msg[0]  = 5;
    // We need to convert the sent reference into 2 bytes
    int new_ref = msg.substring(3).toInt();
    i2c_msg[1]  = new_ref / 256;
    i2c_msg[2]  = new_ref - i2c_msg[1] * 255;  
    return;
  }

  // Command 10 [C10] - Reset Error
  // Command format:      C10
  // Command example:     C10
  // Command description: When driver receives this command, it resets the error
  // Command in I2C:      {10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
  if (msg.startsWith("C10") == true)
  {
    sprintln("Received command C10 to reset all errors");
    new_command =  true;
    i2c_msg[0]  = 10;
    return;   
  }

  // Command 13 [C13] - Serial Data Stream
  // Command format:      C13 1/0
  // Command example:     C13 1
  // Command description: When driver receives this command, it enables or disables the serial data stream
  // Command in I2C:      {13, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
  if (msg.startsWith("C13 ") == true)
  {
    sprintln("Received command C13.");
    ResetMsg();
    // We need to check whether we want to enable or disable the serial data stream
    if (msg.startsWith("C13 1") == true)
    {
      // Enabling the Serial Data Stream
      i2c_msg[0]  = 13;
      i2c_msg[1]  = 1;
      new_command = true;
      return;
    }
    else if (msg.startsWith("C13 0") == true)
    {
      // Disabling the Serial Data Stream
      i2c_msg[0]  = 13;
      i2c_msg[1]  = 0;
      new_command = true;
      return;
    }
    // If we're here, that means that the command was sent properly
    sprintln("Command C13 was sent with a bad argument, double check the code.");
    return;
  }
  
  // Command 1 [C1] - Send force references to driver 1
  // Command format:      C1 M1 XXXX M2 XXXX
  // Command example:     C1 M1 0500 M2 1230
  // Command description: When driver receives this command, it changes the references for the motors
  // Command in I2C:      {1, X1, X2, Y1, Y2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} 
  /*
  if (msg.startsWith("C1") == true)
  {
    if(DRV1 == false)
    {
      sprintln("This driver is not enabled in the code, check the system and the top of this code");
      return;
    }
    ResetMsg();
    i2c_msg[0] = 1; // Changing the first byte to 1 to signalize the command

    // We have to get the references for the motors from the message
    // References are integers, their value represent the reference in grams
    int ref1 = 0;
    int ref2 = 0;
    int ref1_pos, ref2_pos;
    ref1_pos = msg.indexOf("M1");
    ref2_pos = msg.indexOf("M2");
    // Check if we've found the M1 and M2 in the message
    if(ref1_pos == -1 || ref2_pos == -1)
    {
      sprintln("Message is not formatted properly, check the message again");
      return;
    }
    // Getting the references out of the message
    ref1 = msg.substring(ref1_pos + 2, ref2_pos).toInt();
    ref2 = msg.substring(ref2_pos + 2).toInt();
    // Debug print for the references
    sprintln("Reference 1: ");
    sprintln((String)ref1);
    sprintln("Reference 2: ");
    sprintln((String)ref2);
    // Since the values can go above 255, we need to convert the number into 2 bytes
    // Conversion of number x into 2 bytes goes like this: x / 256, x - 255 * (x / 256)
    int ref11, ref12, ref21, ref22;
    ref11 = ref1 / 256;
    ref12 = ref1 - 255 * ref11;
    ref21 = ref2 / 256;
    ref22 = ref2 - 255 * ref21;
    // Now we pack that into the message like this: {0, ref11, ref12, ref21, ref22, 0, 0,...}
    i2c_msg[1] = ref11;
    i2c_msg[2] = ref12;
    i2c_msg[3] = ref21;
    i2c_msg[4] = ref22;
    // At this point, we can send out the I2C message
    sprintln("Sending the I2C message to Driver 1 to change the motor references");
    sprintln("Ref11:");
    sprintln((String)ref11);
    sprintln("Ref12:");
    sprintln((String)ref12);
    sprintln("Ref21:");
    sprintln((String)ref21);
    sprintln("Ref22:");
    sprintln((String)ref22);
    SendI2CMessage(1, i2c_msg);
    return;
  }*/

  sprintln("This command is not configured yet");
}
