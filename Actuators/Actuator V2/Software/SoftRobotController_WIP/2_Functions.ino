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
