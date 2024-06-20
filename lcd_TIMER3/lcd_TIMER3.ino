#define SR 8
#define TIMER 9

#define A 7
#define B 6
#define C 5
#define D 4
#define E 3
#define F 2
#define G 1

#define D1 10
#define D2 11
#define D3 12
#define D4 13

int outputPins[] = {
  A, B, C, D, E, F, G
};


int dataPins[] = {
  D1, D2, D3, D4
};

byte outputArray[] = {
  B11111100, // zero
  B01100000, // one
  B11011010, // two
  B11110010, // three
  B01100110, // four
  B10110110, // five
  B10111110, // six
  B11100000, // seven
  B11111110,// eight
  B11110110// nine
};

bool state1 = false, state2 = false, ledState = false, output = false, timer = false, backward = false;
static bool prevState1 = false, prevState2 = false;
int count = 0;
int limit = 10;
unsigned long time = 0;
static unsigned long prevTime = 0;

void setup() {
 for (unsigned int i = 0; i < sizeof(outputPins); ++i) {
  pinMode(outputPins[i], OUTPUT);
  digitalWrite(i, false);
 }

 for (unsigned int i = 0; i < sizeof(dataPins); ++i) {
  pinMode(dataPins[i], OUTPUT);
  digitalWrite(i, false);
 }

 pinMode(SR, INPUT_PULLUP);
 pinMode(TIMER, INPUT_PULLUP);

 DDRB |= B00000000;
 DDRD |= B11111111;
}

void loop() {
  state1 = !digitalRead(SR);
  state2 = !digitalRead(TIMER);

  if (state1 && !prevState1) {
    output = !output;
  }

  if (output) {

    if (state2 && !prevState2) {
      timer = !timer;
    } 

    if (timer) {
      time = millis();

      if (time - prevTime >= 1000) {
        prevTime = time;

        if (!backward) {
          if (count < limit) {
            count++;
          } else {
            count = 0;
          }
        } else {
          if (count > 0) {
            count--;
          } else {
            count = limit;
          }
        }
      }
    }

    byte currentOutput = outputArray[count];

    for (unsigned int i = 0; i < sizeof(outputPins); ++i) {
      //digitalWrite(outputPins[i], bitRead(currentOutput, i));

       if (bitRead(currentOutput, i)) {
        PORTD |= (1 << outputPins[i]); // Set the bit high
      } else {
        PORTD &= ~(1 << outputPins[i]); // Set the bit low
      }

    }
  } else {
      PORTD &= B00000000;
  }

  prevState1 = state1;
  prevState2 = state2;
}

// 0 on D1- digit 1
// 0 on D2- digit 2, etc
// so to choose the digit displayed you should give low state output on given digit
