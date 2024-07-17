/*
 *      FUNCTIONS
 */

// Function for the debug print on the Serial
void sprint(String msg)
{
  if(SERIAL_DEBUG == true) Serial.print(msg);
  return;
}

// Function for the debug print on the Serial
void sprintln(String msg)
{
  if(SERIAL_DEBUG == true) Serial.println(msg);
  return;
}

// Function for reseting the message
void ResetMsg()
{
  for (int i = 0; i < LEN; i++) i2c_msg[i] = 0;
  return;
}

// Function for blinking the LED-s in the start up sequence
void StartSequence()
{
  delay(STARTSEQ_WAIT);
  delay(STARTSEQ_WAIT);
  digitalWrite(PIN_LED_HEARTBEAT, LOW);
  delay(STARTSEQ_WAIT);
  digitalWrite(PIN_LED_FAULT,     LOW);
  delay(STARTSEQ_WAIT);
  digitalWrite(PIN_LED_ENABLE,    LOW);
  delay(STARTSEQ_WAIT);
  digitalWrite(PIN_LED_MODE,      LOW);
  delay(STARTSEQ_WAIT);
  digitalWrite(PIN_LED_MODE,      HIGH);
  delay(STARTSEQ_WAIT);
  digitalWrite(PIN_LED_ENABLE,    HIGH);
  delay(STARTSEQ_WAIT);
  digitalWrite(PIN_LED_FAULT,     HIGH);
  delay(STARTSEQ_WAIT);
  digitalWrite(PIN_LED_HEARTBEAT, HIGH);
  delay(STARTSEQ_WAIT);
  digitalWrite(PIN_LED_HEARTBEAT, LOW);
  digitalWrite(PIN_LED_FAULT,     LOW);
  digitalWrite(PIN_LED_ENABLE,    LOW);
  digitalWrite(PIN_LED_MODE,      LOW);
  delay(STARTSEQ_WAIT);
  digitalWrite(PIN_LED_HEARTBEAT, HIGH);
  digitalWrite(PIN_LED_FAULT,     HIGH);
  digitalWrite(PIN_LED_ENABLE,    HIGH);
  digitalWrite(PIN_LED_MODE,      HIGH);
  delay(STARTSEQ_WAIT);
  digitalWrite(PIN_LED_HEARTBEAT, LOW);
  digitalWrite(PIN_LED_FAULT,     LOW);
  digitalWrite(PIN_LED_ENABLE,    LOW);
  digitalWrite(PIN_LED_MODE,      LOW);
  delay(STARTSEQ_WAIT);
  digitalWrite(PIN_LED_HEARTBEAT, HIGH);
  digitalWrite(PIN_LED_FAULT,     HIGH);
  digitalWrite(PIN_LED_ENABLE,    HIGH);
  digitalWrite(PIN_LED_MODE,      HIGH);
}

// LED Control Functions
void FaultLED(int flag)
{
  if (flag == 1)
  {
    digitalWrite(PIN_LED_FAULT, LOW);
    return;
  }
  else if (flag == 0)
  {
    digitalWrite(PIN_LED_FAULT, HIGH);
    return;
  }
  sprintln("Error in FaultLED function call, flag out of range.");
  return;
}

void EnableLED(int flag)
{
  if (flag == 1)
  {
    digitalWrite(PIN_LED_ENABLE, LOW);
    return;
  }
  else if (flag == 0)
  {
    digitalWrite(PIN_LED_ENABLE, HIGH);
    return;
  }
  sprintln("Error in EnableLED function call, flag out of range.");
  return;
}

void Error(int flag)
{
  if (flag == 1)
  {
    // DisableDriver();
    // KillPWM();
    error = 1;
    sprintln("Driver has went into Error state, disabling driver and killing PWM!");
    FaultLED(1);
    return;
  }
  else if (flag == 0)
  {
    sprintln("Resetting the error state on the controller!");
    error = 0;
    FaultLED(0);
    return;
  }
  sprintln("Flag for the Error Function is out of range, check code!");
  return;
}

// This function is called when an emergency STOP is needed
// It needs to:
// 1. Disable the driver
// 2. Set PWM to 0% duty cycle
// 3. Turn the Fault LED ON
void EmergencyStop()
{
  driver_enable = false;
  UpdateDriver(driver_enable);
  //KillPWM();
  Error(1);
}

// Function for checking if there is any Serial data
void CheckSerial()
{
  if (Serial.available() <= 0) return;
  serial_msg  = "";
  new_message = true;
  while (Serial.available() > 0)
  {
    char c = (char)Serial.read();
    if (c != '\n' && c != '\r') serial_msg = serial_msg + c;
  }
  return;
}

// Function for Enabling / Disabling the Driver
void UpdateDriver(bool en_flag)
{
  if (en_flag == true)
  {
    // Enable the driver, this includes these things:
    // 1. Set Reset Pin to HIGH
    // 2. Set Sleep Pin to HIGH
    // 3. Turn ON Enable LED
    digitalWrite(PIN_DRV_RESET, HIGH);
    digitalWrite(PIN_DRV_SLEEP, HIGH);
    EnableLED(1);
    return;
  }
  else
  {
    // Disable the driver, this includes these things:
    // 1. Set Reset Pin to LOW
    // 2. Set Sleep Pin to LOW
    // 3. Turn OFF Enable LED
    // 4. Reset the PI accumulator acc
    digitalWrite(PIN_DRV_RESET, LOW);
    digitalWrite(PIN_DRV_SLEEP, LOW);
    EnableLED(0);
    acc = 0;
    return;
  }
}

// Function for updating the decay mode on the DRV8842
void DecayMode(int dm)
{
  if (dm == 1)
  {
    // When switching to fast decay mode, set the decay pin to HIGH
    digitalWrite(PIN_DRV_DECAY, HIGH);
    return;
  }
  else if (dm == 0)
  {
    // When switching to slow decay mode, set the decay pin to LOW
    digitalWrite(PIN_DRV_DECAY, LOW);
    return; 
  }
  // If we're here there was an error with the argument
  sprintln("Bad function call for the function DecayMode, check code");
}

// Function for sending a new command to the motor driver
void MotorDriver(int new_direction, int new_dutycycle)
{
  if (new_dutycycle < 0 || new_dutycycle > 100)
  {
    // Speed out of range
    sprintln("ERROR - Duty cycle out of range for the motor");
    return;
  }
  if (new_direction == 1)
  {
    // Always first kill the PWM on the unused channel
    PWM_Instance[0]->setPWM(PIN_MOTOR_PWM_1, PWM_FREQUENCY, 0.00);
    PWM_Instance[1]->setPWM(PIN_MOTOR_PWM_2, PWM_FREQUENCY, new_dutycycle);
  }
  else if (new_direction == 2)
  {
    // Always first kill the PWM on the unused channel
    PWM_Instance[1]->setPWM(PIN_MOTOR_PWM_2, PWM_FREQUENCY, 0.00);
    PWM_Instance[0]->setPWM(PIN_MOTOR_PWM_1, PWM_FREQUENCY, new_dutycycle);
  }
  else
  {
    // If we're here, the direction is not in range
    sprintln("ERROR - Direction out of range for the motor");
    return;
  } 
}

// Function for printing out the data stream on the serial port
void PrintDataStream()
{
  //Serial.print("MotorCurrent:");
  //Serial.print(motor_current);
  //Serial.print(",INA125:");
  //Serial.print(ina125);
  //Serial.print("Reference:");
  //Serial.print(reference);
  //Serial.print(",INA826:");
  //Serial.println(ina826g);
  serial_msg = "$" + String(reference) + "," + String(ina826g) + "&" + String(millis()) + "#" + String(pi_duty) + "%";
  Serial.println(serial_msg);
  return;
}
