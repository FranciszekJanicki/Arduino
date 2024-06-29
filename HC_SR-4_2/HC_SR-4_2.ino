#include <time.h>

#define trigPin 10
#define echoPin 11
#define outputButton 9
#define measureButton 8
#define LED_SR 12
#define LED1 7
#define LED2 6
#define LED3 5
#define LED4 4
#define LED5 3

bool output = false, measure = false, ledState_SR = false;
bool outputState = false, measureState = false;
static bool prevOutputState = false, prevMeasureState = false;
unsigned long ctime = 0;
unsigned long distance = 0;
const int soundSpeed = 340;
time_t currentTime, prevTime;

int digitalInputPins[] = {
  echoPin,
  outputButton,
  measureButton
};

int digitalOutputPins[] = {
  trigPin,
  LED1, 
  LED2,
  LED3,
  LED4,
  LED5,
};

bool ledStates[] = {
  false, 
  false,
  false,
  false
};

const unsigned int distanceArray[] = {
  5, 10, 15, 20, 25
};

// void setLeds(bool ledArray[], const unsigned int& distance, const unsigned int distArray[]) {
//   for (size_t i = 0; i < sizeof(ledArray); ++i) {
//     if (distArray[i] <= distance) {
//       ledArray[i] = true;
//     } else {
//       ledArray[i] = false;
//     }
//   }
// }

// OR

void setLeds(bool (*ledArray)[], const unsigned int& distance, const unsigned int *distArray) {
  for (size_t i = 0; i < sizeof(*ledArray) / sizeof((*ledArray)[0]); ++i) { // sizeof returns multiplication of sizef of all dimensions, meaning for 2D rows*columns, so when trying to get size of first dimension you should use
  // sizeof(array) / sizeof(array[0]), when trying to get size of second dimension you should use sizeof(array[0])!!!
  
    ledArray[i] = distance >= distArray[i]; // if distance is higher than i distance Interval set it to true, else set it to false
  }
}

unsigned long measureStateTime(const unsigned int& pin, const bool& state) {
  unsigned long ctime = 0;

  if (state) {
    if (!digitalRead(pin)) time = millis();
    else ctime = 0;
  
  } else {
    if (digitalRead(pin)) time = millis();
    else ctime = 0;
  }
  return ctime;
}

unsigned int getDistance(const int& soundSpeed) {
  digitalWrite(trigPin, false);
  delayMicroseconds(2);
  digitalWrite(trigPin, true);
  delayMicroseconds(10);
  digitalWrite(trigPin, false);
  
  ctime = pulseIn(echoPin, HIGH);

  // return (time/2.0)/1000000.0 * soundSpeed; 
  return ctime / 58;
}


void setup() {
    for (const int& pin : digitalInputPins) {
      pinMode(pin, INPUT_PULLUP);
    }

  for (const int& pin : digitalOutputPins) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, false);
  }

  Serial.begin(9600);
  time(NULL);
}

void loop() {
  outputState = !digitalRead(outputButton);
  measureState = !digitalRead(measureButton);

  if (outputState && !prevOutputState)  {
    output = !output;
  }

  if (output) {
    ledState_SR = true;

    if (measureState && !prevMeasureState)  {
        measure = !measure;
    }
    
    currentTime = time();

    if (currentTime - prevTime >= 100) {
      prevTime = time;
  
    if (measure) {
      distance = getDistance(soundSpeed);
      Serial.println((int)distance);

    } else {
       memset(ledStates, 0, sizeof(ledStates)); // set elements of array to specific value
    }
    }
    
    setLeds(&ledStates, distance, distanceArray);

  } else {
    memset(ledStates, 0, sizeof(ledStates)/sizeof(ledStates[0]));
    ledState_SR = false;
  }

  for (unsigned int i = 1; i < sizeof(digitalOutputPins)/sizeof(digitalOutputPins[0]); ++i) {
      digitalWrite(digitalOutputPins[i], ledStates[i-1]);
  }

  prevOutputState = outputState;
  prevMeasureState = measureState;
}