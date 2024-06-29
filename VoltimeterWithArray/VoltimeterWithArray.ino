#define A5 19

size_t size = 0;
int i = 0;
float *values = new float[size];
float value = 0;
bool currentSet = false, alarm = false, output = false;
static bool previousSet = false;
void setup() {
  pinMode(13, INPUT_PULLUP);
  pinMode(A5, INPUT);
  pinMode(12, OUTPUT);

  Serial.begin(9600);
  digitalWrite(12, false);
}

void copyAndResize(float *array, size_t &size) {

  float *newArray = new float[size * 2]; // can't return pointers to variables existing only in scope (function). The solution is to create either static variable 
  // and return pointer to it (or first element of array) or dynamically allocate memory and return pointer to it!
  size *= 2;

  // copy memory from old to new (reallocate)
  for (size_t i = 0; i < size; ++i) {
    newArray[i] = array[i]; 
  }
  // OR
  //memcpy(array, copy, size * sizeof(float));

  array = newArray;
  delete[] newArray;

  return array; // return array that contains old array but with 2*indexes
}

void loop() {
  currentSet = !digitalRead(13);

  if (!previousSet && currentSet) {
    output = !output;
  }

  if (output) {
    value = analogRead(A5) * (5.0/1024.0); // .0 is information for compilator to treat it as float, if it were (5/1024) it would be rounded to int

    if (i >= size) {
      copyAndResize(values, size);
    } // only allocate new memory when it's needed

    values[i] = value; // save new value, it will execute 9 times and only 10th the previous if statement will be excetuted 
    // this way you only allocate memory and copy pointers to large arrays every 10 iterations
    Serial.println(value);
    i++; // loop function acts as internal loop, thus needed iterator
  }
  
  if (value >= 3) {
    if(!alarm) {
    alarm = true;
    Serial.println("3 Volts exceeded!");
    }
    else {
      alarm = false;
    }
  }
  else {
    alarm = false;
  }
  digitalWrite(12, alarm);

  delay(1000);
  previousSet = currentSet;
}