# Solar Power Monitor
![Bench View](images/Bench_view_thumb.jpg)
This is an experimental device to measure solar panel performance. It is adapted from the stacked version of the wireless solar-powered weather station kit available from **IoT Kits** at [w4krl.com/iot-kits](https://w4krl.com/iot-kits/). [![w4krl_iot-kits](https://img.shields.io/badge/W4KRL-IoT%20Kits-brightgreen.svg?style=flat)](https://w4krl.com/iot-kits/)

It uses the Wemos D1 Mini with Arduino software. Solar panel output is measured by an INA219 voltage/current sensor module. A BH1750 module measures light intensity.
<!-- a normal html comment -->
The unit periodically transmits the measurements to your ThingSpeak channel using RESTful architecture with HTTP protocol. It goes into deep sleep between transmissions to reduce energy consumption. 

The D1 Mini receives its energy from the collected solar power. If the solar power is not sufficient to recharge the LiPo cell in darkness the D1 Mini can be powered with an external 5 Volt USB adapter.

Click for project photos: [![photo gallery](https://img.shields.io/badge/Photo-Gallery-brightgreen.svg?style=flat)](https://github.com/W4KRL/SolarPowerMonitor/blob/master/Photo_Gallery.md)

Live data can be viewed when teh station is online at https://thingspeak.com/channels/190041. 

## Installing the Firmware
Installation of the Arduino IDE and ESP8266 is described in many tutorials available online. The main thing to note is that the switch on the charger shield must be in the RUN position to operate and the opposite position when uploading firmware to the D1 Mini.

Use the following as a guide:
* Install the latest version of the Arduino IDE as appropriate to your computer available from [www.arduino.cc](https://www.arduino.cc). Do not install the web version.
* Install the ESP8266 core into the Arduino IDE. See instructions at  https://github.com/esp8266/Arduino#installing-with-boards-manager
* Use the Arduino IDE menu item *Tools | Board:* to select **LOLIN(WEMOS) R2 D1 & mini** or whatever version of the ESP8266 you are using.  
* Download the D1M-Solar-Monitor firmware from this repository. Click on the **Clone or download** button and select **Download ZIP**. Unzip the file. It contains a folder SolarPowerMonitor-master. That folder contains several folders and files. Copy folders **D1M-Solar-Power-Monitor** and **D1M_calibration** to your Arduino folder. [![firmware_download](https://img.shields.io/badge/Download-Illustration-brightgreen.svg?style=flat)](/images/Firmware_download.png)
* Use Arduino IDE menu item *Sketch | Include Library | Manage libraries...* to add these sensor libraries:
   - **hp_BH1750** by Stefan Armborst
   - **Adafruit_INA219** by Adafruit
* Open a ThingSpeak channel at [www.ThingSpeak.com](https://thingspeak.com/) and configure it as described in the ThingSpeak Field Definitions table below.
* Enter your Wi-Fi and ThingSpeak information in the ThingSpeak_config.h file as described below.
* Save your sketch. Connect your computer to the D1 Mini micro USB port. Verify that the D1 Mini serial port is recognized and set *Tools | Port* accordingly.
* Set the switch on the charger board to the position opposite the **RUN** position. This is the **PROG** position but it is unlabelled.
* Upload the firmware to the D1 Mini. Set the switch to **RUN** when the upload finishes. You can view useful information from the unit on the IDE serial monitor.

### ThingSpeak Field Definitions
| Field | Name |
| - | ------------- |
| 1 | Light Intensity |
| 2 | Panel Volts |
| 3 | Panel mA |
| 4 | Panel mW |
| 5 | Cell Volts |
| 6 | WiFi Signal |
 *  Check only used fields 1 through 6
 *  Check "Show Location" if you wish
 *  Leave Show Status unchecked
 
### Configuring ThingSpeak_config.h
The ThingSpeak_config.h file defines firmware parameters that are unique to your solar power monitor. It must be located in the same folder as the sketch D1M-Solar-Monitor.ino file.

Open the sketch in the Arduino IDE. Select the ThingSpeak_config.h tab and edit the information for your station as indicated by the comments within the file.

Information needed:
1. Your WiFi SSID (You must use 2.4 GHz not 5 GHz.)
2. Your WiFi password
3. Sleep interval in seconds: 60 for testing, 300 or longer for normal service. Shorter intervals use more energy. **ThingSpeak does not permit an interval shorter than 15 seconds.**
4. ThingSpeak channel ID (a numerical value)
5. ThingSpeak API Write Key (alphanumeric between quotes)
6. OPTIONAL (Values determined from running D1M-WX1_Calibration.ino)
   * DMM voltage
   * ADC reading

Save the sketch. Set the **RUN** switch to the **PROG** (unlabeled) position and upload to the microcontroller. Return the switch to **RUN** after a sucessful upload.

## How it works
The solar panel produces power depending upon the brightness of the light falling on the panel. Current flows from the positive terminal of the panel to the Vin+ terminal of the INA219 current/voltage sensor. It flows out of the Vin- terminal to the IN+ terminal of the TP4056 charge controller. The current from the BAT+ terminal charges the LiPo cell and returns to the negative terminal of the solar panel.

The INA219 measures the current flow from the solar panel to the TP4056 and measures the voltage at the Vin- terminal to ground. This is the same voltage that appears at the TP4056 IN+ terminal. The INA219 also calculates the power from the solar panel. These measurements are sent to the D1 Mini over the I2C bus.

The TP4056 charge controller passes all current from the solar panel to the Lithium Polymer cell until the terminal voltage of the cell reaches 4.2 Volts at full charge. At that voltage, it limits current to prevent overcharge of the cell.

The LiPo cell supplies power to the ESP8266 microcontroller through Schottky diode D1 and a low dropout voltage regulator on the D1 Mini module. The regulator provides 3.3 volts to the ESP8266 whenever the cell voltage is above that voltage. The D1 Mini can also operate on 5 volt power from the micro USB port. Diode D1 prevents overcharge of the LiPo cell when the D1 Mini is connected to a USB supply.

#### Simplified Power Supply Schematic
![](images/EnergyFlow.png)

The Arduino firmware in the ESP8266 runs in a perpeptual cycle as long as the LiPo cell can provide approximately 3.3 volts or more:
* Initialize sensors (INA219 and BH1750)
* Logon to your WiFi
* Read sensors: 
   - Light intensity
   - Panel voltage, current & power
   - LiPo cell voltage
   - WiFi received signal strength
* Print data to your serial port
* Post data to your ThingSpeak channel
* Enter deep sleep for one to five minutes 

## Complete Schematic
![](images/INA219_Solar_Schematic.jpg)

## MATLAB Scripts
These MATLAB scripts are for your ThingSpeak channel:

### Light_Power_Correlation
This visualization plots 24-hours of power and light intensity on a scatter chart. The relationship between power ouput in milliwatts and light intensity in kilolux is printed on the chart. The script discriminates between the charging and maintenance actions of the TP4056 charge controller.Charging data is displayed as red circles, maintenance data as green circles. The equation is fitted only to charging data.

Light & Power Correlation [![light_power_correlation](https://img.shields.io/badge/MATLAB-Script-brightgreen.svg?style=flat)](https://github.com/W4KRL/SolarPowerMonitor/blob/master/MATLAB_Scripts/Light_Power_Correlation)

![](images/light_power_correlation.png)

### Light and Power Overlayed

This visualization plots the power and light intensity with time over 24-hours. It displays the energy collected over the 24-hour period as milliwatt-hours.

Light & Power Overlayed [![light_power_overlay](https://img.shields.io/badge/MATLAB-Script-brightgreen.svg?style=flat)](https://github.com/W4KRL/SolarPowerMonitor/blob/master/MATLAB_Scripts/Light_Power_Overlayed.txt)

![](images/Power_and_light_overlayed.png)

### Current Values

This displays the current values and the energy collected over the previous 24 hours.

Current Values [![Current_values](https://img.shields.io/badge/MATLAB-Script-brightgreen.svg?style=flat)](https://github.com/W4KRL/SolarPowerMonitor/blob/master/MATLAB_Scripts/Current_Values)

![](images/Current_Values.png)
