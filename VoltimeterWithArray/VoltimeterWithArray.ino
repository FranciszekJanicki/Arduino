#define A5 19

int size = 0, sizeAdder = 10;
int i = 0;
float* values = new float[size];
float value = 0;
bool currentSet = false, previousSet = false, output = false;
bool alarm = false;

void setup() {
  pinMode(13, INPUT_PULLUP);
  pinMode(A5, INPUT);
  pinMode(12, OUTPUT);

  Serial.begin(9600);
  digitalWrite(12, false);
}

float* copyAndResize(float array[], const int& sizeAdder) {
  int size = sizeof(array);
  float* copy = array;

  array = new float[size + sizeAdder]; // pointing to other variable
  // you can't define variables with name of existing variable
  for (unsigned int i = 0; i < size; ++i) {
    array[i] = copy[i]; 
  }
  delete[] copy;

  return array; // return array that contains old array but with 10 new indexes
}

void loop() {
  currentSet = !digitalRead(13);

  if (!previousSet && currentSet) {
    output = !output;
  }

  if (output) {
    value = analogRead(A5) * (5.0/1024.0); // .0 is information for compilator to treat it as float, if it were (5/1024) it would be rounded to int

    if (i >= size) {
      values = copyAndResize(values, sizeAdder);
      size += sizeAdder;
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