# home-setup
My home (automation) setup

## Hardware

* Raspberry Pi 4 Model B - 4 GB RAM
  * Serves as home base.
* 3 x ESP8266
  * WIFI enable sensors (placed in different rooms)

Sensors:
* 1 x DHT21 (Humidity Temperature Sensor)
* 3 x BME280 (Barometric Sensor)

## Setup

Running docker on the Raspberry PI:
* [home-assistant](https://github.com/home-assistant/docker)
* [eclipse-mosquitto](https://github.com/eclipse/mosquitto) (MQTT Broker)

The Raspberry PI is connected to the DHT21 sensor. 
The sensor is read via python script and sends data via MQTT (sensor is also autodetected).

The 3 ESP8266 have each a BME280 sensor connected and are configured for MQTT autodetection with home-assistant.
