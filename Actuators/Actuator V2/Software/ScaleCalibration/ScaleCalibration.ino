#define PIN_SENSE_FORCE_1   26
#define PIN_SENSE_FORCE_2   27


int sum = 0;
int avg = 0;
int readings = 50;
float b1 = 1.1181;
float a1 = 201.65977;
float b2 = 0.95124;
float a2 = 620.7782;
float grams;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  analogReadResolution(12);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < readings; i++)
  {
    sum += analogRead(PIN_SENSE_FORCE_2);
    delay(20);
  }
  avg = sum / readings;
  sum = 0;
  Serial.print("Average value measured: ");
  Serial.println(avg);
  
  Serial.print("Value calculated: ");
  grams = (avg - a2) / b2;
  Serial.print(grams);
  Serial.println("g");
  
  delay(1000);
}
