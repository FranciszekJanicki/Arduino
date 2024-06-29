#define trigPin 10
#define echoPin 11
#define outputButton 9
#define measureButton 8
#define LED1 12
#define LED2 13

bool output = false, measure = false;
bool outputState = false, measureState = false;
static bool prevOutputState = false, prevMeasureState = false;
unsigned long time = 0;
unsigned long distance = 0;
const int soundSpeed = 340;

int digitalInputPins[] = {
  echoPin,
  outputButton,
  measureButton
};

int digitalOutputPins[] = {
  trigPin,
  LED1, 
  LED2
};

bool ledStates[] = {
  false, false
};

bool digitalInputStates[sizeof(digitalInputPins)] = {
  false, false, false
};

bool prevDigitalInputStates[sizeof(digitalInputPins)] = {
  false, false, false
};

unsigned long measureStateTime(const unsigned int& pin, const bool& state) {
  unsigned long time = 0;

  if (state) {
    if (!digitalRead(pin)) time = millis();
    else time = 0;
  
  } else {
    if (digitalRead(pin)) time = millis();
    else time = 0;
  }

  return time;
}

unsigned int getDistance(const int& soundSpeed) {
  digitalWrite(trigPin, false);
  delayMicroseconds(2);
  digitalWrite(trigPin, true);
  delayMicroseconds(10);
  digitalWrite(trigPin, false);
  
  time = pulseIn(echoPin, HIGH);

  // return (time/2.0)/1000000.0 * soundSpeed; 
  return time / 58;
}


void setup() {
    for (int& pin : digitalInputPins) {
      pinMode(pin, INPUT_PULLUP);
    }

  for (int& pin : digitalOutputPins) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, false);
  }

  Serial.begin(9600);
}

void loop() {
  outputState = !digitalRead(outputButton);
  measureState = !digitalRead(measureButton);

  if (outputState && !prevOutputState)  {
    output = !output;
  }

  if (output) {
    // measure = false;
    ledStates[0] = true;

    // if (digitalInputStates[2] && !prevDigitalInputStates[2])  {
    //     measure = true;
    //     ledStates[1] = true;
    // }

    //if (measure) {
      distance = getDistance(soundSpeed);
      Serial.println((int)distance);
    //}

  } else {
    for (bool& led : ledStates) {
      led = false;
    }
  }

  for (unsigned int i = 1; i < sizeof(digitalOutputPins)/sizeof(digitalOutputPins[0]); ++i) {
      digitalWrite(digitalOutputPins[i], ledStates[i-1]);
  }

  prevOutputState = outputState;
  prevMeasureState = measureState;
  delay(500);
}