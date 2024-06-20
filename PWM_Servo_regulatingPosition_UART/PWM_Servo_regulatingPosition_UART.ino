#include <Servo.h>
// always supply servo/ DC motor with independent power supply (can be the same, just stabilised)
// if battery is connected to arduino then Vin pin can be used as output
#define servo 10
#define SR 1
#define AUTO 0
#define LED1 2
#define LED2 3

bool state  = false, prevState = false, output = false, backward = false, braking = false, 
ledState1 = false, ledState2 = false, autoState = false, prevAutoState = false, autoMode = false;
Servo servoObj;
int position = 0, step = 5; // position variable acts as PWM Duty, except it's not used for AnalogWrite() so it can be bigger than 100
// step variable is the step of incrementing/decrementing position in each iteration
int reading = 0; // variable read from UART, in nonAutoMode its setting position
int lastAutoPosition = 0; // used to going back to last auto position after disabiling nonAuto mode (potentiometer mode)
int limit = 180; // degrees limit. Could also be a variable defined by ADC input

void setup() {
  pinMode(servo, OUTPUT);
  servoObj.attach(servo); // method for servo class object setting output pin 
  pinMode(SR, INPUT_PULLUP);
  pinMode(AUTO, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  digitalWrite(LED1, false);
  digitalWrite(LED2, false);
  Serial.begin(9600);
}

void loop() {
  state = !digitalRead(SR); // input from switch1
  autoState = !digitalRead(AUTO); // input from switch2

  if (state && !prevState) {
    output = !output;
  }

  if (output) {

    if (!braking) { // move only if theres need to
     if (!backward) { // handling moving with only 1 variable (backward) for all functions logic
      position += step; // incremnting if backward
      } else {
      position -= step; // decremnting if backward
      } // step is the same for incrementing and decrementing so it will go at same speed each way
    }

    if (autoMode) { // autoMode
      braking = false; // no braking in autoMode

      if (position >= limit) { // if above max limit of servo go back
        backward = true;
      } else if (position <= 0) { // when at beginning go forward
        backward = false;
      } 
    } else { // nonAutoMode moving to position
        while (!Serial.available()) {} // wait for  input
        reading = Serial.readStringUntil('\n').toInt();
        while (!(reading <= limit && reading >= 0)) {
            while (!Serial.available()) {} // wait for  input
            reading = Serial.readStringUntil('\n').toInt();
            Serial.println("Wrong number!");
          }

        Serial.println(reading);
        // position = reading; // immediate jump to this position

        if (position < reading) {
          backward = false;
          braking = false;
        } else if (position > reading){
          backward = true;
          braking = false;
        } else { // at position
          braking = true;
        }
    }

    ledState1 = true;
    ledState2 = false; // always reset LED2 as its meant to only blink
    Serial.println(position);

  } else {

    ledState1 = false;
    ledState2 = false;
  }

  if (autoState && !prevAutoState) {
    if (autoMode) {
      lastAutoPosition = position;
    } else {
      position = lastAutoPosition;
    }
    autoMode = !autoMode;
    ledState2 = !ledState2; // toggle on LED2 (is the same as ledState2 = true)
  }

  servoObj.write(position); // write method argument works like PWM's duty except it can be above 100, ex. 0-180, the method does the calculation itself
  digitalWrite(LED1, ledState1);
  digitalWrite(LED2, ledState2);

  delay(100);  
  prevState = state;
  prevAutoState = autoState;
}
