//www.elegoo.com
//2016.12.12

// define the LED digit patterns, from 0 - 9
// 1 = LED on, 0 = LED off, in this order:
//                74HC595 pin     Q0,Q1,Q2,Q3,Q4,Q5,Q6,Q7 
//                Mapping to      a,b,c,d,e,f,g of Seven-Segment LED
byte seven_seg_digits[20] = { B11111100,  // = 0
                              B11111101,  // = 0.5
                              B01100000,  // = 1.0
                              B01100001,  // = 1.5
                              B11011010,  // = 2
                              B11011011,  // = 2.5
                              B11110010,  // = 3
                              B11110011,  // = 3.5
                              B01100110,  // = 4
                              B01100111,  // = 4.5
                              B10110110,  // = 5
                              B10110111,  // = 5.5
                              B10111110,  // = 6
                              B10111111,  // = 6.5
                              B11100000,  // = 7
                              B11100001,  // = 7.5
                              B11111110,  // = 8
                              B11111111,  // = 8.5
                              B11100110,  // = 9
                              B11100111   // = 9.5
                             };
 
// connect to the ST_CP of 74HC595 (pin 3,latch pin)
int latchPin = 3;
// connect to the SH_CP of 74HC595 (pin 4, clock pin)
int clockPin = 4;
// connect to the DS of 74HC595 (pin 2)
int dataPin = 2;

// button input
int buttonPin = 8;
// increase counter on button press
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
int index = 0;
int cadence;

unsigned long lastTenTimes[10]; //time storage
unsigned long currentTime;
 
void setup() {
  Serial.begin(9600);
  // Set latchPin, clockPin, dataPin as output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  //button
  pinMode(buttonPin, INPUT);

  //pulse LED
  pinMode(LED_BUILTIN, OUTPUT);
}
 
void pulse()
{
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                       // dwell
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                       // dwell
}

// display a number on the digital segment display
void sevenSegWrite(int digit) {
  // set the latchPin to low potential, before sending data
  digitalWrite(latchPin, LOW);
     
  // the original data (bit pattern)
  shiftOut(dataPin, clockPin, LSBFIRST, seven_seg_digits[digit]);  
 
  // set the latchPin to high potential, after sending data
  digitalWrite(latchPin, HIGH);
}

int detectOneButtonPress(int currentButtonState, int previousButtonState)
{
  // compare the buttonState to its previous state
  if (currentButtonState != previousButtonState) {
    // if the state has changed, increment the counter
    if (currentButtonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      Serial.println("on");
      return 1;
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  return 0;
}

unsigned long averageTime(unsigned long currentTime, unsigned long timeTenPressesAgo)
{
  unsigned long timeDifference = currentTime - timeTenPressesAgo;
  return timeDifference/10;
} 

int convertCadenceToSegInput(int candence)
{
  if(cadence == 100)
  {
    return 19;
  }
  else
  {
    return cadence / 5;
  }
}

int calculateSegInput(int averageLastTenPresses)
{
  if(averageLastTenPresses < 600)
  {
    cadence = 100;
    return convertCadenceToSegInput(cadence);
  }
  //95 632ms per press
  if(averageLastTenPresses >= 600 && averageLastTenPresses < 632)
  {
    cadence = 95;
    return convertCadenceToSegInput(cadence);
  }
  //90 666ms per press
  if(averageLastTenPresses >= 632 && averageLastTenPresses < 666)
  {
    cadence = 90;
    return convertCadenceToSegInput(cadence);
  }
  //85 705ms per press
  if(averageLastTenPresses >= 666 && averageLastTenPresses < 705)
  {
    cadence = 85;
    return convertCadenceToSegInput(cadence);
  }
  //80 750ms per press
  if(averageLastTenPresses >= 705 && averageLastTenPresses < 750)
  {
    cadence = 80;
    return convertCadenceToSegInput(cadence);
  }
  //75 800ms per press
  if(averageLastTenPresses >= 750 && averageLastTenPresses < 800)
  {
    cadence = 75;
    return convertCadenceToSegInput(cadence);
  }
  //70 857ms per press
  if(averageLastTenPresses >= 800 && averageLastTenPresses < 857)
  {
    cadence = 70;
    return convertCadenceToSegInput(cadence);
  }
  //65 923ms per press
  if(averageLastTenPresses > 857)
  {
    cadence = 65;
    return convertCadenceToSegInput(cadence);
  }

  return 0;
}

void loop() {
  // read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);
  unsigned long timeTenPressesAgo;
  unsigned long averageLastTenPresses;
  //Add one to buttonPushCounter if button press detected
  if(detectOneButtonPress(buttonState, lastButtonState) == 1)
  {
    currentTime = millis();
    timeTenPressesAgo = lastTenTimes[index];
    lastTenTimes[index] = currentTime;
    // Serial.println(lastTenTimes[index]);
    if(index == 9)
    {
      index = 0;
    }
    else 
    {
      index++;
    }
    Serial.println("average time");
    averageLastTenPresses = averageTime(currentTime, timeTenPressesAgo);
    Serial.println(averageLastTenPresses);
  }
  
  //Take difference in time between 'last button press' and '10 presses before'
  //chart for bpm
  //100 600ms per press
  int segInput = calculateSegInput(averageLastTenPresses);

  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;

  sevenSegWrite(segInput);
}
