volatile int prevFirstSensor = 0;    // first analog sensor
volatile int prevSecondSensor = 0;   // second analog sensor
volatile int firstSensor = 0;    // first analog sensor
volatile int secondSensor = 0;   // second analog sensor
volatile unsigned long startTime = 0;    // start time of stopwatch in milliseconds
volatile unsigned long elapsedTime = 0;  // stopwatch time in milliseconds
int inByte = 0;         // incoming serial byte
volatile bool startFromFirstSensor = true;

void setup() {
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
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
    if ((prevFirstSensor > 512) && (prevSecondSensor > 512))  {
      // wait for start
      if (firstSensor < 512) {
        startFromFirstSensor = true;
        startTime = millis();
      } else if (secondSensor < 512) {
        startFromFirstSensor = false;
        startTime = millis();
      }
    }
  } else if (startTime > 0) {
    // check finish
    if (startFromFirstSensor) {
      if (secondSensor < 512) {
        startTime = 0;
      }
    } else {
      if (firstSensor < 512) {
        startTime = 0;
      }
    }
  }
  prevFirstSensor = firstSensor;
  prevSecondSensor = secondSensor;
  
  while (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    switch (inByte) {
    case 'c':
    case 'C':
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
      break;
    case 'r':
    case 'R':
      startTime = 0;
      elapsedTime = 0;
      break;
    default:
      break;
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
