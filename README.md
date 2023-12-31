# GistinhoProject

This repository contains the source code for the Gistinho project, that was submitted to Hackaday's 2023 Halloween Contest. Gistinho is a skull on a mannequin that can play sounds and blink his eyes in different colors. He can be controlled by a web page and was implemented using an Arduino and some NodeMCUs. He also doubles as a temperature and humidity sensor. He's sure to cause a few spooks!

## Authors ##
1. Cris Guimarães
2. João Gabriel Peixoto
3. Luiz Fernando Costa

## Project Structure and Brief Explanation ##
The project is divided in two main frameworks/platforms, Arduino and NodeMCU (with the respective folders). 
NodeMCU related source code is under the NodeMCU folder, that is divided in two subfolders:
* transmitter: the NodeMCU device responsible for reading the sensor data and sending it through MQTT
  - init.lua: this file is executed at startup every time the NodeMCU device is reset, it calls the setup routines after a timer to prevent issues
  - sensors.lua: this file is dedicated to sensor readings, both the LM35 (temperature) and DHT11 (humidity and temperature, but used only for humidity), aswell as ADC setup for LM35 reading
  - setupwifi.lua: this file is responsible for setting up the WiFi connection, the WiFi's SSID and password need to be updated to a nearby network for it to be used 
* receiver: the NodeMCU device responsible for receiving the sensor data through MQTT, sending it to a HTTP server and communicating through I2C with the Arduino portion
  - init.lua: this file is executed at startup every time the NodeMCU device is reset, and it calls the setup routines after a timer to prevent issues
  - setupwifi.lua: this file is responsible for setting up the WiFi connection, the WiFi's SSID and password need to be updated to a nearby network for it to be used. It also creates a network you can connect to, with the SSID and password provided by the user

Arduino related source code is under the Arduino folder, divided in two subfolders:
* main: the main Arduino application code
  - main.ino: this file is executed at startup every time the Arduino device is reset. It contains all the main code logic, including blinking the leds and playing sounds
* SchedulerVTimer: the SchedulerVTimer library, a scheduler and virtual timer library written for microcontroller applications, with the ATMega328p specific component ([this library is hosted here](https://github.com/joaogpd/SchedulerVTimer))

## Components Used ##
The components used will be divided by whether used in tandem with the Arduino or with the NodeMCUs.
* Arduino Uno R2 by Arduino:
  - DFMini Player by DFRobot
  - two speakers
  - four NeoPixel LEDs by NeoPixel
  - PIR Sensor by Adafruit
  - microSD Card
  - resistors
* D1Mini by Wemos (NodeMCU):
  - LM35 (analog temperature sensor) by Texas Instruments
  - DHT11 (digital temperature and humidity sensor)  
