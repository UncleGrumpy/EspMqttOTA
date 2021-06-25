#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Updater.h>
#include "conf.h"
#include <PipedStream.h>


#define LED_PIN         0
#define BROKER_ADDR     IPAddress(192,168,0,124)
//#define WIFI_SSID       "ssid"
//#define WIFI_PASSWORD   "pass"
#define UPDATE_TOPIC    "esp01t/update/+"
//#define ATOMIC_FS_UPDATE
#define MQTT_MAX_TRANSFER_SIZE 478200

PipedStreamPair pipes(MQTT_MAX_TRANSFER_SIZE);
PipedStream& buffMQ = pipes.first;
PipedStream& buffUP = pipes.second;

WiFiClient wireless;
PubSubClient updater;

void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  // This callback is called when message from MQTT broker is received.
  // Please note that you should always verify if the message's topic is the one you expect.
  // For example: if (memcmp(topic, "myCustomTopic") == 0) { ... }
  if (length == 0) {
    return;
  }
  Serial.print("New message on topic: ");
  Serial.println(topic);
  Serial.print("Size: ");
  Serial.println(length);
  //Serial.println((const char*)payload);
  Update.setMD5(topic);
  if (Update.begin(length)) {
    unsigned int startTime = millis();
    if (Update.writeStream(buffUP) == length) {
      Serial.println("Clearing retained message.");
      updater.publish(UPDATE_TOPIC, "");
      unsigned int exTime = millis() - startTime;
      Serial.printf("Update Success: %u\nRebooting...\n", exTime);
      Update.end();
    } else {
      Serial.println("Failed to apply update!");
      Update.printError(Serial);
      Serial.println("Clearing retained message.");
      updater.publish(UPDATE_TOPIC, "");
    }
  } else {
  Serial.print("Not enough free space (");
  Serial.print(ESP.getFreeSketchSpace());
  Serial.print(") for sketch (");
  Serial.print(length);
  Serial.println(").");
  Serial.println("Clearing retained message.");
  updater.publish(UPDATE_TOPIC, "");
  }
}

void onMqttConnectionFailed() {
    Serial.println("Failed to connect to the update broker!");
}

//PubSubClient updater(BROKER_ADDR, 1883, onMqttMessage, client, *stream);

void setup() {
    Serial.begin(9600);
    delay(50);
    Serial.println("Starting...");
    buffMQ.write(0);
    Update.runAsync(true);

    // Unique ID must be set!
    byte mac[WL_MAC_ADDR_LENGTH];
    WiFi.macAddress(mac);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // connect to wifi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500); // waiting for the connection
    }
    Serial.println();
    Serial.println("Connected to the network");

    updater.setClient(wireless);
    updater.setServer(BROKER_ADDR, 1883);
    updater.setCallback(onMqttMessage);
    updater.setBufferSize(478200);
    updater.setStream(buffMQ);
    updater.connect("ESP01-test-update-client");

    if (! updater.connected()) {
      onMqttConnectionFailed();
    }
    Serial.println("Update client connected to broker.");
    if (! updater.subscribe(UPDATE_TOPIC)) {
      Serial.print("Failed to subscribe to ");
      Serial.println(UPDATE_TOPIC);
    }
    Serial.print("Subscribed to ");
    Serial.println(UPDATE_TOPIC);
}

void loop() {
  if (! updater.loop()) {
    //Serial.println("Update client failure!");
    if (! updater.connect("ESP01-test-update-client")) {
      Serial.println("Failed to reconnect to broker!");
      return;
    }
    if (! updater.subscribe(UPDATE_TOPIC)) {
      Serial.print("Failed to subscribe to ");
      Serial.println(UPDATE_TOPIC);
      updater.loop();
    }
  }
}
