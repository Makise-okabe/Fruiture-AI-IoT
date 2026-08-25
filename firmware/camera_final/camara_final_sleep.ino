#include "esp_camera.h"
#include "Arduino.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"
#include "Base64.h"
#include <WiFi.h>
#include "ESP32MQTTClient.h"

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqttURI = "mqtt://broker.hivemq.com:1883";
const char* publishTopic = "fruiture/Base64image";

ESP32MQTTClient mqttClient;

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#define PHOTO_INTERVAL 20000UL
unsigned long lastPhotoTime = 0;

camera_config_t config;
sensor_t* s;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32-CAM Banana Vision (Stable Edition)");
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM; config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM; config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM; config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM; config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM; config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM; config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM; config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM; config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed!");
    while (true);
  }

  s = esp_camera_sensor_get();
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  WiFi.setSleep(true);

  mqttClient.enableDebuggingMessages();
  mqttClient.setKeepAlive(120);
  mqttClient.setURI(mqttURI);
  mqttClient.enableLastWillMessage("fruiture/status", "ESP32-CAM offline");
  mqttClient.loopStart();
}

void loop() {
  if (millis() - lastPhotoTime >= PHOTO_INTERVAL) {
    takePhotoAndSend();
    lastPhotoTime = millis();
  }
}

void takePhotoAndSend() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Capture failed; restarting camera...");
    esp_camera_deinit();
    delay(200);
    esp_camera_init(&config);
    return;
  }

  size_t imageLen = fb->len;
  String encoded = base64::encode(fb->buf, fb->len);
  size_t base64Len = encoded.length();
  esp_camera_fb_return(fb);
  Serial.printf("Image size %d bytes | Base64 %d\n", imageLen, base64Len);

  if (!mqttClient.isConnected()) {
    mqttClient.loopStart();
    delay(500);
  }

  bool sent = false;
  for (int i = 1; i <= 3 && !sent; i++) {
    sent = mqttClient.publish(publishTopic, encoded.c_str(), 0, false);
    if (!sent) delay(800);
  }

  s->set_gain_ctrl(s, 0);
  s->set_exposure_ctrl(s, 0);
  s->set_awb_gain(s, 0);
  delay(PHOTO_INTERVAL - 2000);
  s->set_gain_ctrl(s, 1);
  s->set_exposure_ctrl(s, 1);
  s->set_awb_gain(s, 1);
}

void onMqttConnect(esp_mqtt_client_handle_t client) {}
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5,0,0)
esp_err_t handleMQTT(esp_mqtt_event_handle_t event) { mqttClient.onEventCallback(event); return ESP_OK; }
#else
void handleMQTT(void* h, esp_event_base_t b, int32_t id, void* d) {
  auto* event = (esp_mqtt_event_handle_t)d;
  mqttClient.onEventCallback(event);
}
#endif
