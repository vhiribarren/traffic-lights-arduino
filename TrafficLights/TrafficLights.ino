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

#define MODULE_MOSFET // Select MODULE_MOSFET or MODULE_RELAY

#ifdef MODULE_RELAY
#define INVERT_LIGHT_OUTPUT
#endif

const int GPIO_BUTTON = 2;
const int GPIO_INTERNAL_LED = 13;
const int GPIO_LIGHT_GREEN =  10;
const int GPIO_LIGHT_YELLOW = 11;
const int GPIO_LIGHT_RED = 12;

const int DURATION_MAIN_LOOP = 100;

const int DURATION_BLINK = 1000;
const int DURATION_REGULAR_GREEN = 25000;
const int DURATION_REGULAR_YELLOW = 5000;
const int DURATION_REGULAR_RED = 25000;

#ifndef INVERT_LIGHT_OUTPUT
const int LIGHT_ON = HIGH;
const int LIGHT_OFF = LOW;
#else
const int LIGHT_ON = LOW;
const int LIGHT_OFF = HIGH;
#endif

const int SWITCH_ON = LOW;
const int SWITCH_OFF = HIGH;
const int INTERNAL_LED_ON = HIGH;
const int INTERNAL_LED_OFF = LOW;

void operationAll();
void operationBlink();
void operationRegular();

int operationIndex = 0;
bool operationReset = true;
const int OPERATION_TABLE_LEN = 3;
void (*operationTable[])() = {operationAll, operationRegular, operationBlink};


void lightAll() {
  digitalWrite(GPIO_LIGHT_GREEN, LIGHT_ON);
  digitalWrite(GPIO_LIGHT_YELLOW, LIGHT_ON);
  digitalWrite(GPIO_LIGHT_RED, LIGHT_ON);
}

void lightNone() {
  digitalWrite(GPIO_LIGHT_GREEN, LIGHT_OFF);
  digitalWrite(GPIO_LIGHT_YELLOW, LIGHT_OFF);
  digitalWrite(GPIO_LIGHT_RED, LIGHT_OFF);  
}

void lightGreen() {
  digitalWrite(GPIO_LIGHT_GREEN, LIGHT_ON);
  digitalWrite(GPIO_LIGHT_YELLOW, LIGHT_OFF);
  digitalWrite(GPIO_LIGHT_RED, LIGHT_OFF);  
}

void lightYellow() {
  digitalWrite(GPIO_LIGHT_GREEN, LIGHT_OFF);
  digitalWrite(GPIO_LIGHT_YELLOW, LIGHT_ON);
  digitalWrite(GPIO_LIGHT_RED, LIGHT_OFF);  
}

void lightRed() {
  digitalWrite(GPIO_LIGHT_GREEN, LIGHT_OFF);
  digitalWrite(GPIO_LIGHT_YELLOW, LIGHT_OFF);
  digitalWrite(GPIO_LIGHT_RED, LIGHT_ON);  
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
  static int previousValue = SWITCH_OFF;
  int buttonValue = digitalRead(GPIO_BUTTON);
  if (buttonValue == SWITCH_OFF) {
    digitalWrite(GPIO_INTERNAL_LED, INTERNAL_LED_OFF);
    if (previousValue != buttonValue) {
      operationNext();
    }
  } else { // Button pushed
    digitalWrite(GPIO_INTERNAL_LED, INTERNAL_LED_ON);
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
  pinMode(GPIO_BUTTON,INPUT_PULLUP);
  pinMode(GPIO_LIGHT_GREEN, OUTPUT);
  pinMode(GPIO_LIGHT_YELLOW, OUTPUT);
  pinMode(GPIO_LIGHT_RED, OUTPUT);
  pinMode(GPIO_INTERNAL_LED, OUTPUT);
  digitalWrite(GPIO_LIGHT_GREEN, LIGHT_OFF);
  digitalWrite(GPIO_LIGHT_YELLOW, LIGHT_OFF);
  digitalWrite(GPIO_LIGHT_RED, LIGHT_OFF);
}


void loop() {
  operationApply();
  checkButton();
  delay(DURATION_MAIN_LOOP);
}
