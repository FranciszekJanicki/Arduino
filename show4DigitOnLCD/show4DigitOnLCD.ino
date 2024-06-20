#include <time.h>
#include <math.h>

typedef u_int8_t byte;

unsigned int randomInt = 0;
unsigned int min = 1000, max = 9999;
unsigned long startTime = 0;
static unsigned long prevTime = 0;
unsigned long delay_ = 1000;
unsigned int digitArray[4];// array of digits starting with MSD

/*
int* arrayOfPtrs = new int[n]; array of initialized pointers to int

int* arrayOfPtrs[n]; array with only first element initialized as ptr to int
for (int i = 0; i < n; ++i) arrayOfPtrs[i] = new int;  initializing rest of them, now its the same as above

int array[n];
int* ptrToArray = array; pointer to array 
int referenceToArray =& array; reference to array

*/

/*
byte byteArray[] = {
  enter bytes needed for outputting high states and low states to represent ceratin digit on LCD
  start array with 0, end with 9
  of course you need to know which register you are physicailly using, each byte should be based on how you do it
  example:
  B00000000, all pins set to low state, so no number will be shown
  B10000000, first pin set to high state, let's say it's D register on Arduino (pins 7-0), 
  whichever pin on the LCD you connect to the 7th pin (first bit in register) will be set to high, thus selected digit on LCD presenting this certain sign
};
*/

#define A 7
#define B 6
#define C 5
#define D 4
#define E 3
#define F 2
#define G 1

int LCD_pins[] = {
 A, B, C, D, E, F, G
};

#define digit4 13
#define digit3 12
#define digit2 11
#define digit1 10

int digitPins[] = {
  digit4, digit3, digit2, digit1
};

void intToLCD(const unsigned int& fourDigitInt, unsigned int digitArray[]) {
  if (fourDigitInt >= 10000) {
    Serial.println("Number must be 4 digits only!");
  
  } else {
    Serial.println(fourDigitInt);


    for (int i = 3; i >= 0; --i) {
      digitArray[3-i] = (fourDigitInt / static_cast<unsigned int>(pow(10, i)) % 10); // n'th digit = number / 10^n % 10,
    }

    // OR

    for (int i = 0; i < 4; ++i) {
      digitArray[3-i] = (fourDigitInt / static_cast<unsigned int>(pow(10, i)) % 10); // n'th digit = number / 10^n % 10,
    }

    // OR

    for (int i = 0; i < 4; ++i) {
      digitArray[i] = (fourDigitInt / static_cast<unsigned int>(pow(10, i)) % 10); // (4-n)'th digit = number / 10^n % 10,
    }
    // reverse so that MSD is first, LSD is last 
    unsigned int start = 0, end = sizeof(digitArray) - 1, temp = 0;
    while (start < end) {
      temp = digitArray[start];
      digitArray[start] = digitArray[end];
      digitArray[end] = temp;

      end--;
      start++;
    }
  }

  unsigned int i = 0;
  for (i = 0; i < 4; ++i) {
    Serial.println(digitArray[i]);
  }

  writeInt(digitArray);
}

byte byteIntArray[] = { // ABCDEFG_ (7 digit LCD)
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

void writeInt(unsigned int digits[]) {
  unsigned int currentDigit = 0;
  byte currentByte = B00000000;
  unsigned int start = sizeof(digits) - 4; // when there are only 3 digits, only 3 last digits will be activated on LCD, etc

  for (unsigned int i = start; i < sizeof(digits)/sizeof(digits[0]); ++i) {
    currentDigit = digits[i];
    currentByte = byteIntArray[currentDigit]; // able to do this because my byteIntArray is exactly from 0 to 9, so I can access it with index being my digit to print
    
    for (unsigned int j = 0; j < sizeof(digitPins); ++j)  digitalWrite(digitPins[j], i==j); // choose digit

    PORTD = currentByte; // print number

    delayMicroseconds(100);
  }
}

byte byteCharArray[] = {
  // byte for 'a'
  // 'b'
  // 'c'
  // 'd'
  // 'e'
  //etc
};

char signArray[] = {
  'a',
  'b',
  'c',
  'd',
  'e'
  // etc
};

void writeChar(const char signs[]) {
  byte currentByte = B00000000;
  char currentSign = ' ';

  for (unsigned int i = 0; i < sizeof(signs)/sizeof(signs[0]); ++i) {
    digitalWrite(digitPins[i], true);

    currentSign = signs[i];

    for (unsigned int k = 0; k < sizeof(signArray)/sizeof(signArray[0]); ++k) {
      if (currentSign == signArray[k]) {
        currentByte = byteCharArray[k];
        break;
      }
    }

    for (unsigned int j = 0; j < sizeof(digitPins); ++j)  digitalWrite(digitPins[j], i==j); // choose digit
  
    PORTD = currentByte; // print char

  /* C++ solution
  // instead of 2 arrays you can use an array of pairs in cpp, because pairs can be of different types
  std::vector <std::pair <char, byte>> pairArray = {
    {'a', B00000000}, // 'a' and byte to write 'a'
    {'b', B00000000}, 
    // etc
  };

  if (signs[i] == pairArray[i].first) {
    PORTD = pairArray[i].second;
  }
  */

    delay(10);
  }
}

unsigned int getRandomInt(const unsigned int& min, const unsigned int& max) {
  char exception = 'e';
  if (min > max) {
    //throw exception;
  } else {
    return min + rand() % (max-min+1);
  }
}

void setup() {
  Serial.begin(9600);
  srand(time(NULL));
}

void loop() {

  startTime = millis();
  if (startTime - prevTime >= delay_) {
    prevTime = startTime;

    //try {
      randomInt = getRandomInt(min, max);
    //} catch (const char& e) {
      //Serial.println(e);
    //}
    intToLCD(randomInt, digitArray);
  } 
  
}
