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
 
const int gpioButton = 2;
const int gpioInternalLed = 13;
const int gpioLightGreen =  10;
const int gpioLightYellow = 11;
const int gpioLightRed = 12;

const int DURATION_MAIN_LOOP = 100;

const int DURATION_BLINK = 2000;
const int DURATION_REGULAR_GREEN = 20000;
const int DURATION_REGULAR_YELLOW = 5000;
const int DURATION_REGULAR_RED = 10000;

const int lightOn = LOW;
const int lightOff = HIGH;

void operationAll();
void operationBlink();
void operationRegular();

int operationIndex = 0;
const int OPERATION_TABLE_LEN = 3;
void (*operationTable[])() = {operationAll, operationBlink, operationRegular};


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


void checkButton() {
  static int previousValue = HIGH;
  int buttonValue = digitalRead(gpioButton);
  if (buttonValue == HIGH) { // Not pushed
    digitalWrite(gpioInternalLed, LOW);
    if (previousValue != buttonValue) {
      operationIndex = (operationIndex + 1) % OPERATION_TABLE_LEN;
    }
  } else { // Pushed
    digitalWrite(gpioInternalLed, HIGH);
  }
  previousValue = buttonValue;
}


void operationAll() {
  lightAll();
}


void operationBlink() {
  static int state = 0;
  static unsigned long timeRef = millis();
  switch(state) {
    case 0: {
      lightYellow();
      unsigned long currentTime = millis();
      if (abs(currentTime - timeRef) > DURATION_BLINK) {
        timeRef = currentTime;
        state = 1;
      }
      break;
    }
    case 1: {
      lightNone();
      unsigned long currentTime = millis();
      if (abs(currentTime - timeRef) > DURATION_BLINK) {
        timeRef = currentTime;
        state = 0;
      }
      break;
    }
  }
}


void operationRegular() {
  static int state = 0;
  static unsigned long timeRef = millis();
  switch(state) {
    case 0: {
      lightGreen();
      unsigned long currentTime = millis();
      if (abs(currentTime - timeRef) > DURATION_REGULAR_GREEN) {
        timeRef = currentTime;
        state = 1;
      }
      break;
    }
    case 1: {
      lightYellow();
      unsigned long currentTime = millis();
      if (abs(currentTime - timeRef) > DURATION_REGULAR_YELLOW) {
        timeRef = currentTime;
        state = 2;
      }
      break;
    }
    case 2: {
      lightRed();
      unsigned long currentTime = millis();
      if (abs(currentTime - timeRef) > DURATION_REGULAR_RED) {
        timeRef = currentTime;
        state = 0;
      }
      break;
    }
  }
}


void loop() {
  operationTable[operationIndex]();
  checkButton();
  delay(DURATION_MAIN_LOOP);
}
