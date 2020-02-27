/* D1M-Solar-Monitor.ino

   (C) Karl Berger, 2020 All Rights Reserved
   2020.02.24

   Adapted from:
   D1 Mini Weather Station (Solar)
   Posts to ThingSpeak using the REST API

   Set serial monitor to 115,200 baud

*/
/*_____________________________________________________________________________
   Copyright 2016-2020 Berger Engineering dba IoT Kits

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

   https://w4krl.com
   _____________________________________________________________________________
*/

// 2020.02.27 - changed sleep mode to WAKE_RF_DEFAULT

// ****************************************************************
// ** REMEMBER: Set switch to PROG to program and RUN to operate **
// ****************************************************************

//#include <Wire.h>                    // [builtin] I2C bus
#include <ESP8266WiFi.h>             // [builtin] ESP8266 WiFi
#include <hp_BH1750.h>               // [manager] by Stefan Armborst https://github.com/Starmbi/hp_BH1750
#include <Adafruit_INA219.h>         // [manager] https://github.com/adafruit/Adafruit_INA219

// Place your configuration file in same folder as this sketch
#include "ThingSpeak_config.h"

// *******************************************************
// *********************** GLOBALS ***********************
// *******************************************************

const char  IOT_SERVER[] = "api.thingspeak.com";  // ThingSpeak Server

// structure to hold sensor measurements/calculations
struct {
  float lightLevel;              // light intensity (lux)
  float panelVolts;              // solar panel voltage
  float panelMilliAmps;          // solar panel current
  float panelMilliWatts;         // solar panel power
  float cellVolts;               // LiPo cell voltage
  long  wifiRSSI;                // WiFi signal strength (dBm)
} sensorData;                    // declare struct

// *******************************************************
// ***************** INSTANTIATE OBJECTS *****************
// *******************************************************
hp_BH1750 myBH1750;              // light level sensor
Adafruit_INA219 myINA219;        // voltage/current sensor
WiFiClient client;               // WiFi connection

// *******************************************************
// ************************ SETUP ************************
// *******************************************************
void setup() {
  Serial.begin(115200);          // initialize the serial port
  Serial.println();
  Serial.println();

  //  Wire.begin();                  // wake up the I2C bus for BH1750 & INA219
  initializeSensors();           // start INA219 and BH1750 sensors
  logonToRouter();               // logon to local Wi-Fi
  readSensors();                 // read data into sensorData struct
  printToSerialPort();           // display data on local serial monitor
  postToThingSpeak();            // send data to ThingSpeak
  enterSleep( SLEEP_INTERVAL );  // go to low power sleep mode
} //setup()

// *******************************************************
// ************************* LOOP ************************
// *******************************************************
void loop() {                     // nothing to do here
} // loop()

// *******************************************************
// ***************** SUPPORTING FUNCTIONS ****************
// *******************************************************

// *******************************************************
// ***************** Initialize Sensors ******************
// *******************************************************
void initializeSensors() {
  // check this "TO_GROUND" command. it works but may not be correct or needed
  bool avail = myBH1750.begin(BH1750_TO_GROUND);// init the sensor with address pin connected to ground
  // result (bool) will be be "false" if no sensor found
  myINA219.begin();
  myINA219.setCalibration_16V_400mA();  // set to maximum sensitivity
} // initializeSensors()

// *******************************************************
// ********* Read sensors into sensorData struct *********
// *******************************************************
void readSensors() {
  // read light level in lux
  myBH1750.start();                           // start light measurement
  sensorData.lightLevel = myBH1750.getLux();  // get light measurement
  // read analog voltage from the Analog to Digital Converter
  // on D1 Mini this is 0 - 1023 for voltages 0 to 3.2V
  // the D1M-WX1 has an external resistor to extend the range to 5.0 Volts
  // a fudgeFactor corrects for voltage divider component variation
  // as measured by the user in the calbration step
  float fudgeFactor = dmmVoltage / adcVoltage;
  sensorData.cellVolts = 5.0 * analogRead(A0) * fudgeFactor / 1023.0;
  sensorData.panelVolts = myINA219.getBusVoltage_V();
  sensorData.panelMilliAmps = myINA219.getCurrent_mA();
  sensorData.panelMilliWatts = myINA219.getPower_mW();
  sensorData.wifiRSSI = WiFi.RSSI();  // read the Wi-Fi signal strength (long)
} // readSensors()

// *******************************************************
// ************ Print data to the serial port ************
// *******************************************************
void printToSerialPort() {
  // '\t' is the C++ escape sequence for tab
  // header line
  Serial.println("\n\tlux\tVp\tmA\tmW\tVc\tdBm");
  // data line
  Serial.print("Data\t");
  Serial.print(sensorData.lightLevel,0);
  Serial.print("\t");
  Serial.print(sensorData.panelVolts, 2);
  Serial.print("\t");
  Serial.print(sensorData.panelMilliAmps, 2);
  Serial.print("\t");
  Serial.print(sensorData.panelMilliWatts, 2);
  Serial.print("\t");
  Serial.print(sensorData.cellVolts, 2);
  Serial.print("\t");
  Serial.println(sensorData.wifiRSSI);
  Serial.println("----------------------------------------------------------------------------");
} // printToSerialPort()

// *******************************************************
// ************** Logon to your Wi-Fi router *************
// *******************************************************
void logonToRouter() {
  WiFi.persistent(false);    // prevent it from writing logon to flash memory
  WiFi.mode(WIFI_OFF);
  delay(100);
  int count = 0;
  WiFi.mode(WIFI_STA);
  WiFi.forceSleepWake();     // Bakke
  WiFi.begin( WIFI_SSID, WIFI_PASSWORD );
  while ( WiFi.status() != WL_CONNECTED ) {
    count++;
    // give up if more than 100 tries (10 seconds)
    if ( count > 100) {
      Serial.print("\nWiFi failed. ");
      enterSleep( SLEEP_INTERVAL );   // retry after sleep
      // ****************************************************
      // ** PROCESSING ENDS HERE IF IT FAILS TO CONNECT!!! **
      // ****************************************************
    }
    delay( 500 );                     // ms delay between reports
    Serial.print(".");
  } // loop while not connected
  // WiFi is sucesfully connected
  Serial.print("\nWiFi connected to IP: ");
  Serial.println(WiFi.localIP().toString());
} // logonToRouter()

// *******************************************************
// ********** Post data to ThingSpeak ********************
// *******************************************************
void postToThingSpeak() {
  // assemble and post the data
  if ( client.connect(IOT_SERVER, 80) == true ) {
    Serial.print("ThingSpeak Server connected. ");
    // declare dataString as a String and initialize with the API_WRITE_KEY
    String dataString = API_WRITE_KEY;
    // cocatenate each field onto the end of dataString
    dataString += "&field1=";
    dataString += String(sensorData.lightLevel);
    dataString += "&field2=";
    dataString += String(sensorData.panelVolts);
    dataString += "&field3=";
    dataString += String(sensorData.panelMilliAmps);
    dataString += "&field4=";
    dataString += String(sensorData.panelMilliWatts);
    dataString += "&field5=";
    dataString += String(sensorData.cellVolts);
    dataString += "&field6=";
    dataString += String(sensorData.wifiRSSI);
    //    dataString += "&field7=";
    //    dataString += String();
    //    dataString += "&field8=";
    //    dataString += String();
    //    dataString += "&status=";
    //    dataString += unitStatus;
    //    Serial.println(unitStatus);   // show status on local serial monitor

    // find the number of characters in dataString
    String dataStringLength = String(dataString.length());

    // post the data to ThingSpeak
    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println("X-THINGSPEAKAPIKEY: " + API_WRITE_KEY);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + dataStringLength);
    client.println("");
    client.print(dataString);

    Serial.print("ThingSpeak data sent. ");
  }
  //TODO: remove delay
  delay(500);
  client.stop();
  Serial.println("ThingSpeak disconnected.");
} // postToThingSpeak()

// *******************************************************
// ********************* Enter Sleep *********************
// *******************************************************
void enterSleep(long sleep) {
  WiFi.disconnect( true );
  delay(1);
  // sleep is in seconds
  Serial.print("Entering deep sleep for ");
  Serial.print( sleep );
  Serial.println(" seconds.");
  // WAKE_RF_DEFAULTS wakes with WiFi radio ON
  ESP.deepSleep(sleep * 1000000L, WAKE_RF_DEFAULT);
} // enterSleep()

// *******************************************************
// *********************** END ***************************
// *******************************************************
