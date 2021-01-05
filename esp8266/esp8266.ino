#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define room "<room>"

#define wifi_ssid "<wifi_ssid>"
#define wifi_password "<wifi_password>"

#define mqtt_server "<mqtt_ip>"
#define mqtt_user "<mqtt_broker_user>"
#define mqtt_password "<mqtt_broker_password>"

#define mqtt_client "esp8266"

#define SEA_LEVEL_PRESSURE_HPA (1013.25)

Adafruit_BME280 bme;
WiFiClient espClient;
PubSubClient client(espClient);

float temperature, humidity, pressure, altitude;

void setup() {
  Serial.begin(115200);
  delay(100);
  bme.begin(0x76);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setBufferSize(1024);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  String state_topic = "homeassistant/sensor/sensor" + String(room) + "/state";

  String device_name = "ESP8266 " + String(room);
  String device = "{\"name\":\"" + device_name + "\", \"manufacturer\":\"AZDelivery\", \"model\":\"ESP8266 NodeMCU\", \"identifiers\":[\""+device_name+"\"], \"sw_version\":\"20200101\"}";

  String temperature_config_topic = "homeassistant/sensor/sensor" + String(room) + "T/config";
  String temperature_config = "{ \"unique_id\":\"" + String(room) + "-Temperature\", \"device_class\": \"temperature\", \"name\": \"Temperature\", \"state_topic\": \"" + state_topic + "\", \"unit_of_measurement\": \"°C\", \"value_template\": \"{{ value_json.temperature}}\", \"device\": " + device + "}";

  String humidity_config_topic = "homeassistant/sensor/sensor" + String(room) + "H/config";
  String humidity_config = "{ \"unique_id\":\"" + String(room) + "-Humidity\", \"device_class\": \"humidity\", \"name\": \"Humidity\", \"state_topic\": \"" + state_topic + "\", \"unit_of_measurement\": \"%\", \"value_template\": \"{{ value_json.humidity}}\", \"device\": " + device + " }";

  String pressure_config_topic = "homeassistant/sensor/sensor" + String(room) + "P/config";
  String pressure_config = "{ \"unique_id\":\"" + String(room) + "-Pressure\",\"device_class\": \"pressure\", \"name\": \"Pressure\", \"state_topic\": \"" + state_topic + "\", \"unit_of_measurement\": \"hPa\", \"value_template\": \"{{ value_json.pressure}}\", \"device\": " + device + " }";

  String altitude_config_topic = "homeassistant/sensor/sensor" + String(room) + "A/config";
  String altitude_config = "{ \"unique_id\":\"" + String(room) + "-Altitude\", \"name\": \"Altitude\", \"state_topic\": \"" + state_topic + "\", \"unit_of_measurement\": \"m\", \"value_template\": \"{{ value_json.altitude}}\", \"device\": " + device + " }";

  client.publish(temperature_config_topic.c_str(), temperature_config.c_str(), true);
  client.publish(humidity_config_topic.c_str(), humidity_config.c_str(), true);
  client.publish(pressure_config_topic.c_str(), pressure_config.c_str(), true);
  client.publish(altitude_config_topic.c_str(), altitude_config.c_str(), true);
}

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String mqtt_client_name = String(mqtt_client) + "_" + String(room);
    if (client.connect(mqtt_client_name.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

long lastMsg = 0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 60000) {
    lastMsg = now;

    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;
    float altitude = bme.readAltitude(SEA_LEVEL_PRESSURE_HPA);

    String json = "{ \"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + ", \"pressure\": " + String(pressure) + ", \"altitude\": " + String(altitude) + " }";
    Serial.println(json);

    String state_topic = "homeassistant/sensor/sensor" + String(room) + "/state";

    client.publish(state_topic.c_str(), json.c_str(), true);
  }
}