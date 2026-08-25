# MQTT Topics

The prototype uses the public HiveMQ broker at `broker.hivemq.com:1883`.

| Topic | Direction | Purpose |
|---|---|---|
| `fruiture/temp` | sensor -> backend | Temperature |
| `fruiture/humidity` | sensor -> backend | Relative humidity |
| `fruiture/mq2gas` | sensor -> backend | Corrected gas/VOC reading |
| `fruiture/rawgas` | sensor -> backend | Raw gas reading (when enabled) |
| `fruiture/Base64image` | camera -> backend | Base64 JPEG image |
| `fruiture/trend` | sensor -> backend | Additional trend/metadata |
| `fruiture/ml_input` | backend -> broker | Aggregated ML input summary |
| `fruiture/servo_angle` | predictor -> servo ESP32 | JSON prediction + target servo angle |

> Because these are public broker/topic names, use a private broker and unique topic namespace for any real deployment.
