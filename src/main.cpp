#include <WiFi.h>
#include <PubSubClient.h>

// Replace with your WiFi credentials
const char* ssid = "Studs";
const char* password = "Ses@me#24083";

// IP of your PC running Mosquitto broker
const char* mqtt_server = "10.20.0.71"; // Replace with your local IP

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 2 seconds");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int rawVal = analogRead(33); // read from GPIO27
  float arduinoVal = (rawVal / 4095.0) * 1023.0; // normalize to 10-bit like Arduino
  float percent = ((arduinoVal - 565.0) / (780.0 - 565.0)) * 100.0;

  // Clamp the value
  percent = constrain(percent, 0, 100);

  String payload = "{\"moisture\":" + String(percent, 1) + "}";

  client.publish("soil/moisture", payload.c_str());

  Serial.println("Sent: " + payload);

  delay(50); // publish every 5 seconds
}
