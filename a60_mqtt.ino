#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "mqtt_config.h";

const char* mqtt_server = MQTT_HOST;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[10];


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("desklamp", MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");

      // Once connected, publish an announcement...
      client.publish("desklamp", "connected");
      // ... and resubscribe

      client.subscribe("desklamp/power");
      client.loop();
      client.subscribe("desklamp/toggle");
      client.loop();

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print("length: ");
  Serial.print(length);

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if(strcmp(topic, "desklamp/power") == 0) {
      Log("Received mqtt message");
      payload[length] = '\0';
      //byte power = atoi((char *)payload);
      if(strcmp((char *) payload, "ON") == 0) {
          Log("SET CONSTANTCOLOR");
          state = CONSTANTCOLOR;
      }else {
          Log("SET LIGHTSOFF");
          state = LIGHTSOFF;
      }

  }
  if(strcmp(topic, "desklamp/toggle") == 0) {
      if(state == LIGHTSOFF) {
          state = CONSTANTCOLOR;

      }else {
          state = LIGHTSOFF;
      }
  }

}

void setup_mqtt()
{
    delay(500);
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}


void loop_mqtt()
{

    if (!client.connected()) {
        reconnect();
    }else {
        if(prev_state != state) {

            if(state == LIGHTSOFF) {
                Log("LIGHTSOFF");
                client.publish("desklamp/status", "OFF");
            }else if(state == CONSTANTCOLOR) {
                Log("CONSTANTCOLOR");
                client.publish("desklamp/status", "ON");
            }
            prev_state = state;
        }
    }
    client.loop();
}
