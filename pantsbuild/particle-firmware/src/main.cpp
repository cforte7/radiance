#include "Particle.h"
#include "MQTT.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>

#define ANALOG_PIN A1
#define DELAY_SECONDS 1000
#define DEVICE_ID "device_0"
#define TOPIC "db/append/moisture"
#define BROKER_HOST "10.0.0.57"
#define BROKER_PORT 5653
#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)

unsigned long lastSync = millis();
void callback(char* topic, byte* payload, unsigned int length);

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

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

SerialLogHandler logHandler(LOG_LEVEL_INFO);

int val = 0;

char buf[1024];

std::string generateUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        if (i == 8 || i == 12 || i == 16 || i == 20)
            ss << "-";
        ss << std::hex << dis(gen);
    }
    return ss.str();
}


// void sync_time() {
//   if (millis() - lastSync > ONE_DAY_MILLIS) {
//     Particle.publish("Syncing time.");
//     // Request time synchronization from the Particle Device Cloud
//     Particle.syncTime();
//     lastSync = millis();
//     Particle.publish("Successfully synced time.");
//   }
// }
void setup() { 
  Serial.begin(9600);
  Particle.publish("logging", "Setup starting."); 
  // sync_time();
  client.connect(System.deviceID());
  Particle.publish("logging", "Device is connected.");
}

char *create_payload() {
  Particle.publish("logging", "Creating payload.");
  val = analogRead(ANALOG_PIN);
  String device_id = System.deviceID();
  auto current_time = Time.now();

  snprintf(buf, sizeof(buf), "[\"%s\", %ld, %d, \"%s\"]", 
            generateUUID().c_str(), current_time, val, device_id.c_str());
  Particle.publish("logging", "Created payload with snprintf.");
  return buf;
}

void loop() {
  Particle.publish("logging", "Loop starting...");
  if (client.isConnected()) {
    Particle.publish("logging", "Client is connected");
  } else {
    Particle.publish("logging", "Client is NOT connected");
  }
  // sync_time();
  // if (!client.isConnected()) {
  //   Particle.publish("logging", "Client is disconnected. Reconnecting.");
  //   client.connect(System.deviceID());  
  //     Particle.publish("logging", "Client has reconnected.");
  // }
  char * payload = create_payload();
  Particle.publish("logging", "Publishing data to MQTT broker.");
  client.publish(TOPIC, payload);
  Particle.publish("logging", "Delying now.");
  delay(DELAY_SECONDS);
}
