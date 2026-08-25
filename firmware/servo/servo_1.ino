#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
Servo myServo;

const char* subscribeTopic = "fruiture/servo_angle";
const char* feedbackTopic  = "fruiture/servo_feedback";

#define SERVO_PIN 14
int currentAngle = 0;

void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Servo Controller Initializing...");

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  myServo.attach(SERVO_PIN, 550, 2500);
  myServo.write(0); delay(500);
  myServo.write(90); delay(500);
  myServo.write(0); delay(500);
  currentAngle = 0;
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  char msg[length + 1];
  memcpy(msg, payload, length);
  msg[length] = '\0';

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, msg);
  if (error) return;

  int predicted_day = doc["predicted_day"] | -1;
  float confidence = doc["confidence"] | 0.0;
  int target_angle = doc["servo_angle"] | 0;
  const char* timestamp = doc["timestamp"] | "unknown";

  int target_angle_cali = constrain((int)(target_angle * 1.10), 0, 180);
  int step = (target_angle_cali > currentAngle) ? 1 : -1;
  for (int pos = currentAngle; pos != target_angle_cali; pos += step) {
    myServo.write(pos);
    delay(15);
  }
  myServo.write(target_angle_cali);
  currentAngle = target_angle_cali;

  StaticJsonDocument<256> feedback;
  feedback["servo_angle"] = currentAngle;
  feedback["predicted_day"] = predicted_day;
  feedback["confidence"] = confidence;
  feedback["timestamp"] = timestamp;
  feedback["status"] = "completed";

  char feedbackMsg[256];
  serializeJson(feedback, feedbackMsg);
  client.publish(feedbackTopic, feedbackMsg);
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32Servo-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(subscribeTopic);
    } else {
      delay(5000);
    }
  }
}
