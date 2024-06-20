// interruptions- normally enabled
// when disabiling interrputions you can allow the microcontroller to read inputs and do other tasks
// atach interrputs to certain functions
// disable interrupts to execute time-sensitive code
// inside interruptions- based on microcontrollers counter
// outside interruptions- based on state of pins

#define interruptPin 2
#define LED 3

volatile byte state = false;
volatile word steps = 0; // word is 2byte (16bit) data type, can be used for storing 16bit int/ string/ etc.
// volatile data type means this variable will always be read by compilator with most recent value
static unsigned long prevTime;
unsigned long time = 0, delayVibrations = 50; // delayVibrations used for waiting to be sure it's not vibrations on switch
// static keyword means variable doesnt change when escaping a function, the value remains the same during next iteration when coming back to the function. Static variable is also initialized to 0
// T * const ptr - const ptr to T typename
// const T * ptr- > ptr to const T typename

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(interruptPin), onToggle, CHANGE); // thirt argument could be CHANGE/RISING/FALLING. When this action is performed on interruptPin, the function attached to interrupt is run
  // its used to perform your own code during specified interruption. Let's say you are waiting for very important input and cant miss it- input on outside pin interrupts program and allows for atttached function to be called
  // you dont call this function anywhere- when the action specified as third argument happens, it will be called as interrpution
}


void onToggle() {
  time = millis(); // millis return time since calling function its inside, so inside loop its measuring time since calling the loop (beginning of loop) and inside function its measuring time since calling this specific function
  if (time - prevTime <= delayVibrations) return; // if the last time of calling this function was less then delayVibrations then it wasnt really caused by 'CHANGE' on interruptPin and it was a vibration and its escaping function
  // void function can call return; to escape function

  // if it wasnt vibration then actualize time, acknowledge the 'CHANGE' on pin and do code called during interruption
  prevTime = time;
  steps++;
  state = !state;
}

void loop() {

  noInterrupts();
  // code with no default interruptions (can't use time measure functions inside)
  interrupts();

  Serial.println(steps);
  digitalWrite(LED, state);
  
  delay(5000); // 5s delay to see interruptions
}