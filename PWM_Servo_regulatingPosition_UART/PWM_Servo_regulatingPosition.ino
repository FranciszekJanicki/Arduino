#include <Servo.h>
// always supply servo/ DC motor with independent power supply (can be the same, just stabilised)
// if battery is connected to arduino then Vin pin can be used as output
#define servo 10
#define SR 1
#define AUTO 0
#define LED1 2
#define LED2 3
#define POT A5

bool state  = false, prevState = false, output = false, ledState1 = false, ledState2 = false, backwards = false,
dirState = false, prevDirState = false, autoMode = false, autoState = false;
Servo servoObj;
int position = 0, step = 5; // position variable acts as PWM Duty, except it's not used for AnalogWrite() so it can be bigger than 100
// step variable is the step of incrementing/decrementing position in each iteration
int potentiometer = 0; // potentiometer ADC decimal vlaue
int Limit = 180;

void setup() {
  pinMode(servo, OUTPUT);
  servoObj.attach(servo); // method for servo class object setting output pin 
  pinMode(SR, INPUT_PULLUP);
  pinMode(AUTO, INPUT_PULLUP);
  pinMode(POT, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  digitalWrite(LED1, false);
  digitalWrite(LED2, false);
  Serial.begin(9600);
}

void loop() {
  state = !digitalRead(SR); // input from switch1
  autoState =- !digitalRead(AUTO); // input from switch2
  potentiometer = map(analogRead(POT), 0, 1023, 0, 180); // input from potentiometer

  if (state && !prevState) {
    output = !output;
  }

  if (output) {
    if (!autoMode) {
      position = potentiometer;
    } else {
        if (!backwards) {
          position += step; // incremnting if backwards
        } else {
          position -= step; // decremnting if backwards
        } // step is the same for incrementing and decrementing so it will go at same speed each way
    ledState1 = true;
    ledState2 = false; // always reset LED2 as its meant to only blink
    Serial.println(position);
    } 
  } else {
    ledState1 = false;
    ledState2 = false;
  }

  if (autoMode) {
    if (position >= Limit) { // if above max limit of servo go back
      backwards = true;
    } else if (position <= 0) { // when at beginning go forward
      backwards = false;
    } 
  }

  if (autoState && !autoState) {
    autoMode = !autoMode;
    backwards = !backwards;
    ledState2 = !ledState2; // toggle on LED2 (is the same as ledState2 = true)
  }

  servoObj.write(position); // write method argument works like PWM's duty except it can be above 100, ex. 0-180, the method does the calculation itself
  digitalWrite(LED1, ledState1);
  digitalWrite(LED2, ledState2);
  
  prevState = state;
  prevDirState = dirState;
}
