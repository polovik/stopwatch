volatile int prevFirstSensor = 0;    // first analog sensor
volatile int prevSecondSensor = 0;   // second analog sensor
volatile int firstSensor = 0;    // first analog sensor
volatile int secondSensor = 0;   // second analog sensor
volatile int threshold1 = 512;
volatile int threshold2 = 512;
volatile unsigned long startTime = 0;    // start time of stopwatch in milliseconds
volatile unsigned long elapsedTime = 0;  // stopwatch time in milliseconds
int inByte = 0;         // incoming serial byte
volatile bool startFromFirstSensor = true;

void setup() {
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(115200);
  Serial.setTimeout(50);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  establishContact();  // send a byte to establish contact until receiver responds
}

void loop() {
  // read first analog input:
  firstSensor = analogRead(A0);
  // read second analog input:
  secondSensor = analogRead(A1);
  if ((startTime == 0) && (elapsedTime == 0)) {
    // wait for ready to start
    if ((prevFirstSensor > threshold1) && (prevSecondSensor > threshold2))  {
      // wait for start
      if (firstSensor < threshold1) {
        startFromFirstSensor = true;
        startTime = millis();
      } else if (secondSensor < threshold2) {
        startFromFirstSensor = false;
        startTime = millis();
      }
    }
  } else if (startTime > 0) {
    // check finish
    if (startFromFirstSensor) {
      if (secondSensor < threshold2) {
        startTime = 0;
      }
    } else {
      if (firstSensor < threshold1) {
        startTime = 0;
      }
    }
  }
  prevFirstSensor = firstSensor;
  prevSecondSensor = secondSensor;
  
  while (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    if ((inByte >= 32) && (inByte < (32 + 50))) {
      threshold1 = 1024 * (inByte - 32) / 50;
    } else if ((inByte >= 82) && (inByte < (82 + 50))) {
      threshold2 = 1024 * (inByte - 82) / 50;
    } else if ((inByte == 10) || (inByte == 13)) { // Enter
      // update elapsed time:
      if (startTime > 0) {
        elapsedTime = millis() - startTime;
      }
      // send sensor values:
      Serial.print(firstSensor);
      Serial.print(",");
      Serial.print(secondSensor);
      Serial.print(",");
      Serial.println(elapsedTime);
    } else if (inByte == 8) { // Backspace
      startTime = 0;
      elapsedTime = 0;
    }
  }
  delay(10);
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("0,0,0");   // send an initial string
    delay(300);
  }
}
