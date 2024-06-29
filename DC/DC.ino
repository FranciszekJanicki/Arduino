#define motor_input1 0
#define motor_input2 1
#define LED1 2
#define LED2 3
#define SR_button 4
#define DIR_button 5
#define motor_PWM 6
#define potentiometer A1

unsigned int outputPins[] = {
  motor_input1,
  motor_input2,
  motor_PWM,
  LED1,
  LED2
};

unsigned int inputPins[] = {
  SR_button,
  DIR_button,
  potentiometer
};

bool forwardArray[] = {
  false, 
  true,
};

bool ledArray[] = {
  false,
  false
};

bool output = false, forward = true, left = false, right = false, acc = false, constSpeed = false, changedDirection = false;
bool outputState = false, dirState = false;
static bool prevOutputState = false, prevDirState = false, prevForward = false; 
unsigned int iteration = 0;
unsigned int motor_duty = 0;
unsigned int step = 5, limit = 255;
unsigned int value = 0;
bool iterationEnd = false;

void updateDirection(const bool& forward_value) {
  if (forward_value) {
    left = false;
    right = true;
  } else {
    left = true;
    right = false;
  }
}

void setup() {
  for (unsigned int& pin: outputPins) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, false);
  }
  for (unsigned int& pin: inputPins) {
    pinMode(pin, INPUT_PULLUP);
  }
}

void loop() {
  outputState = !digitalRead(inputPins[0]);
  dirState = !digitalRead(inputPins[1]);
  value = map(analogRead(inputPins[2]), 0, 1023, 0, limit);

  if (outputState && !prevOutputState) {
    output = !output;
  }

  if (output) {
    ledArray[0] = true;
    ledArray[1] = false;


    forward = forwardArray[iteration]; // check direction for current iteration
    
    if (forward != prevForward) { // if direction was changed then start changing direction
      changedDirection = false;  
    }

    if (!changedDirection) { // if not finished changing direction then decelerate
      acc = false;

    } else { // if finished changing direction then update direction 
      updateDirection(forward);

      if (motor_duty < value) { // accelerate in new direction
        acc = true;
        constSpeed = false;
      } else if (motor_duty > value) { // decelerate in new direction (if the speed value was decreased after accelerating over it). It's not mandatory in a sequence steering
        acc = false;
        constSpeed = false;
      } else { // reached intended speed
        constSpeed = true;
        iterationEnd = true; // used for sequence, after accelerating to max speed the iteration increments
      } 
    }


    if (!constSpeed) {
      if (acc) motor_duty += step; // accelerating 

      else { // decelerating

      if (!changedDirection) { // decelerating because of change of directions
        if (motor_duty > 0) motor_duty -= step; // decelerate while over 0
        else changedDirection = true; // at 0 means it stopped and we must change direction in another iteration of main loop
    
      } else { // decelerating because of cecline of speed value
        if (motor_duty > value) motor_duty -= step; // decelerate while over value
        else constSpeed = true; // stay at intended speed value

      }

    }

  } else {
    for (bool& led: ledArray) {
      led = false;
    }

    left = false;
    right = false;
  }

  for (unsigned int i = 3; i < sizeof(outputPins)/sizeof(outputPins[0]); ++i) {
    digitalWrite(outputPins[i], ledArray[i]); 
  }

  digitalWrite(outputPins[0], left);
  digitalWrite(outputPins[1], right);
  analogWrite(outputPins[2], motor_duty);

  prevOutputState = outputState;
  prevDirState = dirState;
  prevForward = forward;

  if (iterationEnd) {  // if end of previous iteration then increment
    if (iteration < sizeof(forwardArray)/sizeof(forwardArray[0])) {
      iteration++;
    } else {
      iteration = 0;
    }
    iterationEnd = false;
  }
}
