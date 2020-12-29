#!/usr/bin/env python3
import paho.mqtt.client as paho
from subprocess import check_output
from re import findall
import json
import Adafruit_DHT
import requests

DHT_SENSOR = Adafruit_DHT.DHT22
DHT_PIN = 23

client = paho.Client()
client.username_pw_set("<mqtt_broker_user>", "<mqtt_broker_password>")
client.connect("localhost", 1883)
client.loop_start()

local_weather_station_url = "https://openweathermap.org/data/2.5/weather/?appid=<app_id>&lat=<lat>&lon=<long>&units=metric"


def get_outside():
    response = requests.get(local_weather_station_url)
    if response.status_code == 200:
        return json.loads(response.content.decode('utf-8'))["main"]
    else:
        return None


def get_cpu_temp():
    cpu_temperature = check_output(["vcgencmd", "measure_temp"]).decode("UTF-8")
    return findall("\d+\.\d+", cpu_temperature)[0]


degree_sign = u"\N{DEGREE SIGN}"

cpu_temp_config = {
    "unique_id": "Raspberry-Temperature",
    "device_class": "temperature",
    "name": "CPU Temperature",
    "state_topic": "homeassistant/sensor/sensorRaspberry/state",
    "unit_of_measurement": degree_sign + "C",
    "value_template": "{{ value_json.cpu_temperature}}",
    "device": {
        "name": "Home Raspberry Pi",
        "manufacturer": "Raspberry",
        "model": "4B",
        "identifiers": ["Home Raspberry"],
        "sw_version": "20200101"
    }
}
client.publish("homeassistant/sensor/sensorRaspberryCpu/config", json.dumps(cpu_temp_config), 0, True)

temperature_config = {
    "unique_id": "LivingRoom-Temperature",
    "device_class": "temperature",
    "name": "Temperature",
    "state_topic": "homeassistant/sensor/sensorRaspberry/state",
    "unit_of_measurement": degree_sign + "C",
    "value_template": "{{ value_json.temperature}}",
    "device": {
        "name": "Home Raspberry Pi",
        "manufacturer": "Raspberry",
        "model": "4B",
        "identifiers": ["Home Raspberry"],
        "sw_version": "20200101"
    }
}
client.publish("homeassistant/sensor/sensorLivingRoomT/config", json.dumps(temperature_config), 0, True)

humidity_config = {
    "unique_id": "LivingRoom-Humidity",
    "device_class": "humidity",
    "name": "Humidity",
    "state_topic": "homeassistant/sensor/sensorRaspberry/state",
    "unit_of_measurement": "%",
    "value_template": "{{ value_json.humidity}}",
    "device": {
        "name": "Home Raspberry Pi",
        "manufacturer": "Raspberry",
        "model": "4B",
        "identifiers": ["Home Raspberry"],
        "sw_version": "20200101"
    }
}
client.publish("homeassistant/sensor/sensorLivingRoomH/config", json.dumps(humidity_config), 0, True)

temp = get_cpu_temp()
humidity, temperature = Adafruit_DHT.read_retry(DHT_SENSOR, DHT_PIN)

data_set = {
    "cpu_temperature": float(temp),
    "temperature": float("{:.2f}".format(temperature)),
    "humidity": float("{:.2f}".format(humidity + 25))
}
client.publish("homeassistant/sensor/sensorRaspberry/state", json.dumps(data_set), 0, True)

outside = get_outside()
if outside is not None:
    outside_temperature_config = {
        "unique_id": "Outdoor-Temperature",
        "device_class": "temperature",
        "name": "Temperature",
        "state_topic": "homeassistant/sensor/sensorOutdoorWeatherStation/state",
        "unit_of_measurement": degree_sign + "C",
        "value_template": "{{ value_json.temperature}}",
        "device": {
            "name": "Outdoor Weather Station",
            "manufacturer": "OpenWeatherMap",
            "model": "Open",
            "identifiers": ["Outdoor Weather Station"],
            "sw_version": "20200101"
        }
    }
    client.publish("homeassistant/sensor/sensorOutdoorWeatherStationT/config", json.dumps(outside_temperature_config),
                   0, True)

    outside_humidity_config = {
        "unique_id": "Outdoor-Humidity",
        "device_class": "humidity",
        "name": "Humidity",
        "state_topic": "homeassistant/sensor/sensorOutdoorWeatherStation/state",
        "unit_of_measurement": "%",
        "value_template": "{{ value_json.humidity}}",
        "device": {
            "name": "Outdoor Weather Station",
            "manufacturer": "OpenWeatherMap",
            "model": "Open",
            "identifiers": ["Outdoor Weather Station"],
            "sw_version": "20200101"
        }
    }
    client.publish("homeassistant/sensor/sensorOutdoorWeatherStationH/config", json.dumps(outside_humidity_config), 0,
                   True)

    outside_data = {
        "temperature": outside["temp"],
        "humidity": outside["humidity"]
    }
    client.publish("homeassistant/sensor/sensorOutdoorWeatherStation/state", json.dumps(outside_data), 0, True)
