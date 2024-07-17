void setup() {
  // Setting up Serial Communication
  Serial.begin(SERIAL_BAUDRATE);
  sprintln("Soft Robot Controller V2 has started!");

  // Setting up all of the pins
  

  // Setting up Timers

  // COM Timer (Communication) - Frequency set in the Parameters section
  if (TimerCOM.attachInterruptInterval(PERIOD_TIMER_COM, CommunicationHandler))
  {
    sprint("Starting COM Timer OK, millis() = ");
    sprintln(String(millis()));
  }
  else
    sprintln("Can't set COM Timer. Select another freq. or timer");

  // CTRL Timer (Control) - Frequency set in the Parameters section
  if (TimerCTRL.attachInterruptInterval(PERIOD_TIMER_CTRL, ControlHandler))
  {
    sprint("Starting CTRL Timer OK, millis() = ");
    sprintln(String(millis()));
  }
  else
    sprintln("Can't set CTRL Timer. Select another freq. or timer");

}

void loop() {
  
}
