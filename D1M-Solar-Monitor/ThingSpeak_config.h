// ThingSpeakConfig.h for D1M-WX1_IoT_REST.ino

// This configuration file should reside in the same Arduino
// directory as the weather station file D1M-WX1_IoT_REST.ino.

// *******************************************************
// ********************* WIFI LOGON **********************
// *******************************************************

// ENTER YOUR WI-FI SSID
// YOU MUST USE 2.4 GHz WiFi, NOT 5 GHz
const char WIFI_SSID[] = "DCMNET";

// ENTER YOUR WI-FI PASSWORD
const char WIFI_PASSWORD[] = "0F1A2D3E4D5G6L7O8R9Y";

// *******************************************************
// ****************** STATION FACTORS ********************
// *******************************************************

const float dmmVoltage = 4.12;  // voltage displayed on your digital multimeter 
const float adcVoltage = 4.13;  // voltage reported by the D1 Mini Analog to Digital Converter

// Enter the update interval in seconds
// The interval must be longer than 15 seconds
// Use 60 seconds for testing, 300 for normal use
const long SLEEP_INTERVAL = 60;

// *******************************************************
// ******************** THINGSPEAK ***********************
// *******************************************************

// Open a ThingSpeak account at www.thingspeak.com
/* Define fields:
 *  Field 1 Light Intensity
 *  Field 2 Panel Volts
 *  Field 3 Panel MilliAmps
 *  Field 4 Panel MilliWatts
 *  Field 5 Cell Volts
 *  Field 6 WiFi Signal
 *  Field 7 not used
 *  Field 8 not used
 *  put check only on used fields
 *  Show Location checked if you wish
 *  Leave Show Status unchecked
 */

// ThingSpeak Channel ID & API Write Key
const long CHANNEL_ID = 190041;            // numerical value
const String API_WRITE_KEY = "DPYSLGVHEAECE9FV";  // between quotes
