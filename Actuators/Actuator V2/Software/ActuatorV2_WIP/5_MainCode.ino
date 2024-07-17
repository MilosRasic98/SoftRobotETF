void setup() {
  // Setting up Serial Communication
  Serial.begin(SERIAL_BAUDRATE);
  sprintln("Actuator V2 has started!");

  // Setting up all of the pins
  // Motor Pins
  pinMode(PIN_MOTOR_PWM_1,    OUTPUT);
  pinMode(PIN_MOTOR_PWM_2,    OUTPUT);
  // LED Pins
  pinMode(PIN_LED_HEARTBEAT,  OUTPUT);
  pinMode(PIN_LED_FAULT,      OUTPUT);
  pinMode(PIN_LED_ENABLE,     OUTPUT);
  pinMode(PIN_LED_MODE,       OUTPUT);
  // DRV Control Pins
  pinMode(PIN_DRV_RESET,      OUTPUT);
  pinMode(PIN_DRV_SLEEP,      OUTPUT);
  pinMode(PIN_DRV_DECAY,      OUTPUT);
  // Analog Pins
  pinMode(PIN_MOTOR_CURR,     INPUT);
  pinMode(PIN_INA125,         INPUT);
  pinMode(PIN_INA826,         INPUT);

  // Setting the initial pin state for the LED-s
  digitalWrite(PIN_LED_HEARTBEAT, HIGH);
  digitalWrite(PIN_LED_FAULT,     HIGH);
  digitalWrite(PIN_LED_ENABLE,    HIGH);
  digitalWrite(PIN_LED_MODE,      HIGH);

  // Setting the Analog Read Resolution
  analogReadResolution(12);

  // Run the LED Start Sequence
  StartSequence();

  // Disable the driver
  UpdateDriver(driver_enable);

  // Set the default decay mode
  DecayMode(decay_mode);

  // Setting up Timers
  
  // LED Timer - Frequency set in the Parameters section
  if (TimerLED.attachInterruptInterval(PERIOD_TIMER_LED, HeartbeatLEDHandler))
  {
    sprint("Starting LED Timer OK, millis() = ");
    sprintln(String(millis()));
  }
  else
    sprintln("Can't set LED Timer. Select another freq. or timer");

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

  // SEN Timer (Sense) - Frequency set in the Parameters section
  if (TimerSEN.attachInterruptInterval(PERIOD_TIMER_SEN, SenHandler))
  {
    sprint("Starting SEN Timer OK, millis() = ");
    sprintln(String(millis()));
  }
  else
    sprintln("Can't set CTRL Timer. Select another freq. or timer");

  // Setting up PWM
  PWM_Instance[0] = new RP2040_PWM(PIN_MOTOR_PWM_1, PWM_FREQUENCY, 0.00f);
  PWM_Instance[0]->setPWM();
  PWM_Instance[1] = new RP2040_PWM(PIN_MOTOR_PWM_2, PWM_FREQUENCY, 0.00f);
  PWM_Instance[1]->setPWM();
}

void loop() {
  
}
