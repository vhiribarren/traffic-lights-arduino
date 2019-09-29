/*
Copyright (c) 2019 Vincent Hiribarren

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

// Using an Arduino Uno

const int gpioButton = 2;
const int gpioInternalLed = 13;
const int gpioLightGreen =  10;
const int gpioLightYellow = 11;
const int gpioLightRed = 12;

const int DURATION_MAIN_LOOP = 100;

const int DURATION_BLINK = 1000;
const int DURATION_REGULAR_GREEN = 25000;
const int DURATION_REGULAR_YELLOW = 5000;
const int DURATION_REGULAR_RED = 25000;

const int lightOn = LOW;
const int lightOff = HIGH;
const int switchOn = LOW;
const int switchOff = HIGH;
const int internalLedOn = HIGH;
const int internalLedOff = LOW;

void operationAll();
void operationBlink();
void operationRegular();

int operationIndex = 0;
bool operationReset = true;
const int OPERATION_TABLE_LEN = 3;
void (*operationTable[])() = {operationAll, operationRegular, operationBlink};


void lightAll() {
  digitalWrite(gpioLightGreen, lightOn);
  digitalWrite(gpioLightYellow, lightOn);
  digitalWrite(gpioLightRed, lightOn);
}

void lightNone() {
  digitalWrite(gpioLightGreen, lightOff);
  digitalWrite(gpioLightYellow, lightOff);
  digitalWrite(gpioLightRed, lightOff);  
}

void lightGreen() {
  digitalWrite(gpioLightGreen, lightOn);
  digitalWrite(gpioLightYellow, lightOff);
  digitalWrite(gpioLightRed, lightOff);  
}

void lightYellow() {
  digitalWrite(gpioLightGreen, lightOff);
  digitalWrite(gpioLightYellow, lightOn);
  digitalWrite(gpioLightRed, lightOff);  
}

void lightRed() {
  digitalWrite(gpioLightGreen, lightOff);
  digitalWrite(gpioLightYellow, lightOff);
  digitalWrite(gpioLightRed, lightOn);  
}


void operationApply() {
  operationTable[operationIndex]();
}

void operationNext() {
  operationIndex = (operationIndex + 1) % OPERATION_TABLE_LEN;
  operationReset = true;
  operationApply();
  operationReset = false;
}


void checkButton() {
  static int previousValue = switchOff;
  int buttonValue = digitalRead(gpioButton);
  if (buttonValue == switchOff) {
    digitalWrite(gpioInternalLed, internalLedOff);
    if (previousValue != buttonValue) {
      operationNext();
    }
  } else { // Button pushed
    digitalWrite(gpioInternalLed, internalLedOn);
  }
  previousValue = buttonValue;
}


void waitAndChangeState(unsigned long timeDuration, int newStateIfSuccess, unsigned long * timeRef,  int *currentState) {
  unsigned long currentTime = millis();
  unsigned long timeDelta = abs(currentTime - (*timeRef));
  if (timeDelta > timeDuration) {
    *timeRef = currentTime;
    *currentState = newStateIfSuccess;
  }
}


void operationAll() {
  static int state = 0;
  if (operationReset) {
    state = 0;
  }
  switch(state) {
    case 0: {
      lightAll();
      state = 1;
      break;
    }
    case 1: {
      break;
    }
  }
}


void operationBlink() {
  static int state = 0;
  static unsigned long timeRef = millis();
  if (operationReset) {
    state = 0;
  }
  switch(state) {
    case 0: {
      lightYellow();
      timeRef = millis();
      state = 1;
      break;
    }
    case 1: {
      waitAndChangeState(DURATION_BLINK, 2, &timeRef, &state);
      break;
    }
    case 2: {
      lightNone();
      state = 3;
      break;
    }
    case 3: {
      waitAndChangeState(DURATION_BLINK, 0, &timeRef, &state);
      break;
    }
  }
}


void operationRegular() {
  static int state = 0;
  static unsigned long timeRef = millis();
  if (operationReset) {
    state = 0;
  }
  switch(state) {
    case 0: {
      lightGreen();
      timeRef = millis();
      state = 1;
      break;
    }
    case 1: {
      waitAndChangeState(DURATION_REGULAR_GREEN, 2, &timeRef, &state);
      break;
    }
    case 2: {
      lightYellow();
      state = 3;
      break;
    }
    case 3: {
      waitAndChangeState(DURATION_REGULAR_YELLOW, 4, &timeRef, &state);
      break;
    }
    case 4: {
      lightRed();
      state = 5;
      break;
    }
    case 5: {
      waitAndChangeState(DURATION_REGULAR_RED, 0, &timeRef, &state);
      break;
    }
  }
}


void setup() {
  pinMode(gpioButton,INPUT_PULLUP);
  pinMode(gpioLightGreen, OUTPUT);
  pinMode(gpioLightYellow, OUTPUT);
  pinMode(gpioLightRed, OUTPUT);
  pinMode(gpioInternalLed, OUTPUT);
  digitalWrite(gpioLightGreen, lightOff);
  digitalWrite(gpioLightYellow, lightOff);
  digitalWrite(gpioLightRed, lightOff);
}


void loop() {
  operationApply();
  checkButton();
  delay(DURATION_MAIN_LOOP);
}
