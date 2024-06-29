// some of the functionality isnt implemented yet

#define motor1_input1 0
#define motor1_input2 1
#define motor2_input1 2
#define motor2_input2 3
#define motor1_pwm 5
#define motor2_pwm 6
#define outputButton 7
#define modeButton 8
#define skipButton 9
#define LED1 10
#define LED2 11
#define LED3 12

unsigned int iteration = 0, iteration2 = 0;
const unsigned int amountOfMotors = 2;
unsigned long time = 0;
static unsigned long prevTime = 0;

bool output = false, autoMode = true, finishedIteration = false; // main program logic
bool outputState = false, modeState = false, skipState = false; // inputs
static bool prevOutputState = false, prevModeState = false, prevSkipState = false; // prev state of inputs

bool ledStates[] = {
  false, // LED1
  false,  // LED2
  false // LED3
};

bool acc[] = {false, false}; // motor 1 and motor 2 acc booleans
bool forward[] = {false, false}; // motor 1 and motor 2 forward booleans for simplified handling of left/ right booleans
bool left[] = {false, false}; // motor 1 and motor 2 input1 on H-bridge (left)
bool right[] = {false, false}; // motor 1 and motor 2 input2 on H-bridge (right)
bool changedDirection[] = {false, false}; // motor1 and motor2 just decelerated to 0 and will start acceleratin in new direction
/*
|left|right|forward|motor
| 1  |  0  |   0    |left
| 0  |  1  |   1    |right   
| 0  |  0  |  prev  |stop 
| 1  |  1  |  prev  |brake
*/

unsigned int DUTY[] = {0, 0}, dutyStep = 5, dutyLimit = 255; // motor1 and motor2 PWM parameters

bool accArray[amountOfMotors] = {
  false, false
}; // array of sequences of accelerating/ decelerating
   // true for acceleration and false for decelaration

bool forwardArray[][amountOfMotors] = {
  {false, true}, // !forward1 and forward2
  {true, false} // forward1 and !forward2
}; // array of forward boolean values
   // true means backward is true, false means backward is false
   // forward is right, !forward is left

bool prevForwardArray[][amountOfMotors] = {
  false, 
  false
};

unsigned int outPinArray[] = {
  motor1_input1,
  motor2_input1,
  motor1_input2,
  motor2_input2,
  motor1_pwm,
  motor2_pwm,
  LED1,
  LED2,
  LED3
};

void setup() {
  for (auto pin : outPinArray) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, false);
  }

  pinMode(outputButton, INPUT_PULLUP);
  pinMode(modeButton, INPUT_PULLUP);
  pinMode(skipButton, INPUT_PULLUP);
}

void assignMotorInputs(bool *forwardArr, const bool (*ptrToForwardValsArray)[]) {
  for (unsigned int i = 0; i < amountOfMotors; ++i) {
    forwardArr[i] = (*ptrToForwardValsArray)[i];

    if (forwardArr[i]) {
      left[i] = false;
      right[i] = true;
    } else {
      left[i] = true;
      right[i] = false;
    }

  }
}

void loop() {
  outputState = !digitalRead(outputButton);
  modeState = !digitalRead(modeButton);
  skipState = !digitalRead(skipButton);

  if (outputState && !prevOutputState) {
    output = !output;
  }


  if (output) {
    ledStates[0] = true;
    ledStates[2] = false;

    // if (modeState && !prevModeState) {
    //   autoMode = !autoMode;
    // }

    // if (!autoMode) {
    //   if (skipState && !prevSkipState) {
    //   finishedIteration = true;
    //   ledStates[2] = true;
    //   }
    //   ledStates[1] = false;

    // } else {
    //     ledStates[1] = true;
    // }

 
    for (unsigned int i = 0; i < amountOfMotors; ++i) {

      if (forwardArray[iteration][i] != prevForwardArray[i]) {
        changedDirection[i] = false; // the procedure of changing directions must begin, thus setting it to false
      }

      if (!changedDirection[i]) {
        accArray[i] = false;
      } else {
        assignMotorInputs(forward, &(forwardArray[iteration]));
        accArray[i] = true;
      }


      time = millis();

      if (time - prevTime >= 100) {
        prevTime = time;

        if (accArray[i]) {
          if (DUTY[i] < dutyLimit) DUTY[i] += dutyStep;
          else finishedIteration = true;
        }
        else {
          if (DUTY[i] > 0) DUTY[i] -= dutyStep;
          else {
            changedDirection[i] = true;
          }
        }

      }
    }

  } else {

    for (auto led : ledStates) {
      led = false;
    }

    for (unsigned int i = 0; i < amountOfMotors; ++i) {
      left[i] = false;
      right[i] = false;
    }

  }


  for (unsigned int i = 6; i < sizeof(outPinArray)/sizeof(outPinArray[0]); ++i) {
    digitalWrite(outPinArray[i], ledStates[i]);
  }

  for (unsigned int i = 0; i < 2; ++i) {
      digitalWrite(outPinArray[i], left[i]);
      digitalWrite(outPinArray[i+2], right[i]);
  }

  for (unsigned int i = 0; i < amountOfMotors; ++i) {
    analogWrite(outPinArray[i+4], DUTY[i]);
  }

  if (finishedIteration) {
    if (iteration >= sizeof(forwardArray)/sizeof(forwardArray[0])) {
        iteration = 0;
    } else {
      iteration++; 
    }
    finishedIteration = false;
  }


  for (unsigned int i = 0; i < amountOfMotors; ++i) {
    prevForwardArray[iteration][i] = forwardArray[iteration][i];
  }

  prevOutputState = outputState;
  prevModeState = modeState;
  prevSkipState = skipState;
}
