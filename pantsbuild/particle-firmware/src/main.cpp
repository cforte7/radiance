#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

#include "MQTT.h"
#include "Particle.h"

#define ANALOG_PIN A1
#define DELAY_SECONDS 10000
#define MQTT_RECONNECT_DELAY_SECONDS 10000
#define DEVICE_ID "device_0"
#define TOPIC "db/append/moisture"
#define LOG_TOPIC "testtopic/message"
#define BROKER_HOST "10.0.0.57"
#define BROKER_PORT 5653
#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)

SerialLogHandler logHandler;

unsigned long lastSync = millis();

// placeholder from the library example. Not sure if I really want/need
// to do anything with this.
void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = '\0';

  if (!strcmp(p, "RED"))
    RGB.color(255, 0, 0);
  else if (!strcmp(p, "GREEN"))
    RGB.color(0, 255, 0);
  else if (!strcmp(p, "BLUE"))
    RGB.color(0, 0, 255);
  else
    RGB.color(255, 255, 255);
  delay(1000);
}

MQTT client(BROKER_HOST, BROKER_PORT, callback);

SYSTEM_MODE(AUTOMATIC);

int val = 0;

char buf[1024];

std::string generateUUID() {
  auto randomHex = []() -> char {
    const char hexChars[] = "0123456789abcdef";
    return hexChars[random(0, 16)];
  };

  std::string uuid;
  for (int i = 0; i < 20; ++i) {
    if (i == 8 || i == 12 || i == 16) uuid += "-";
    uuid += randomHex();
  }
  return uuid;
}

void sync_time() {
  if (millis() - lastSync > ONE_DAY_MILLIS) {
    Particle.publish("Syncing time.");
    // Request time synchronization from the Particle Device Cloud
    Particle.syncTime();
    lastSync = millis();
    Particle.publish("Successfully synced time.");
  }
}

void connect_mqtt_broker() {
  while (!client.isConnected()) {
    Log.info("Client is disconnected. Trying to reconnect.");
    bool result = client.connect(System.deviceID());
    if (!result) {
      Log.info("Reconnection failed. Trying again in %d ms.", MQTT_RECONNECT_DELAY_SECONDS);
      delay(MQTT_RECONNECT_DELAY_SECONDS);
    } else {
      Log.info("Reconnection successful.");
    }
  }
}

void setup() {
  Serial.begin(9600);
  Log.info("Setup starting.");
  bool is_connected = client.connect(System.deviceID());
  if (is_connected) {
    Log.info("Initial MQTT successful.");
  } else {
    Log.error("Initial MQTT connection failed!");
  }
}

char* create_payload() {
  Log.info("Creating payload.");
  val = analogRead(ANALOG_PIN);
  time32_t current_time = Time.now();
  const char* uuid = generateUUID().c_str();

  // TODO: figure out why built in device_id function output was mangled
  // and replace primary device with real id
  snprintf(buf, sizeof(buf), "[\"%s\", %ld, %d, \"%s\"]", uuid, current_time,
           val, "primary device");
  Log.info("Created payload: %s.", buf);
  return buf;
}

void loop() {
  Log.info("Loop starting.");
  connect_mqtt_broker();
  sync_time();
  char* payload = create_payload();

  Log.info("Publishing data to MQTT broker.");
  bool pub_res = client.publish(TOPIC, payload, MQTT::QOS1);
  if (!pub_res) {
    // TODO: implement batching for failed attempts to prevent data loss from
    // temporary disconnections
    Log.info("Failed to publish. Executing explicit disconnect from broker.");
    client.disconnect();
  }

  Log.info("Delying now for %d milliseconds.", DELAY_SECONDS);
  delay(DELAY_SECONDS);
}
