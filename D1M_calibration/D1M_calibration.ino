/*
  D1M-WX1-Calibration.ino
  06/06/2019 - changed name to D1M-WX1-Calibration.ino
  02/01/2019 - Moved to GitHub
  01-31/2019 - ON 0.1 seconds, OFF 0.9 seconds
  23 September 2018 - shortened LED flash and off time
  This program is used with IoT Kits weather stations to calibrate the 
  voltage sensor. It also verifies the proper installation of the Arduino IDE 
  and ESP8266 core. It does two things:
  1) It blinks the onboard LED 0.1 seconds on and 0.9 seconds off
  2) It reports the voltage read by the analog to digital converter (ADC)
  The printed circuit switches must be in this position:
     ON
     PROG
  To use:
  1) Open the Serial Monitor by menu item Tools | Serial Monitor or
     type Control+Shift+M or
     click the Serial Monitor icon in the upper right of the IDE window
  2) Change the baud rate in the Serial Monitor window to 115200 baud
  3) Use a digital multimneter (DMM) to read the dc voltage on the CELL terminals
     of the printed circuit board
  4) Write down the voltage reported by the ADC and the voltage measured on the DMM
  Karl Berger
  2017.03.10
  Copyright(c) 2018-2019 Karl W. Berger dba IoT Kits
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

// the setup function runs once
// Initialize serial communications and set the pinmode
void setup() {
  Serial.begin(115200);             // initialize serial at 115,200 baud
  // the builtin LED on the D1 Mini is connected to pin 2
  pinMode(LED_BUILTIN, OUTPUT);     // Set the LED pin to OUTPUT mode
} // setup()

// the loop function runs continuously until power is removed
// read the ADC and blink the onboard LED
void loop() {
  // read the ADC value. it is an integer between 0 and 1023
  int val = analogRead(A0);         // this is an Arduino function
  // convert the ADC value to a floating point decimal from 0 to 5 volts
  // do not remove the .0 in the next line. it is necessary to
  // make the result a float
  float volts = 5.0 * val / 1023.0;
  // print the voltage to the Serial Monitor
  Serial.print("ADC Voltage reads = ");
  Serial.println(volts);

    // turn the LED on and off
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on
  delay(100);                       // Wait 0.1 second (100 milliseconds)
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off
  delay(900);                       // Wait 0.9 second (900 milliseconds)
} // loop()
