# Technical Overview

## Problem
Fruiture is a low-cost AI-IoT prototype for real-time banana ripeness monitoring. It fuses environmental, chemical and visual sensing, sends measurements over MQTT, aggregates them in a Python backend and predicts a Day 1-Day 5 ripeness index.

## Sensing layer
- **DHT11**: temperature and relative humidity.
- **MQ-2**: gas/VOC proxy, read through the ESP32 ADC after warm-up and calibration.
- **ESP32-CAM / OV2640**: periodic 640x480 JPEG capture, Base64-encoded for MQTT transport.

## Vision pipeline
`banana_detector_no_grey.py` converts BGR images to HSV, applies CLAHE brightness normalisation, masks green/yellow/brown/black regions, rejects low-saturation areas, performs morphological cleanup and contour extraction, and derives RGB/color-proportion features plus a 0-100 visual ripeness score.

## Data / ML pipeline
The final ML feature vector is:

`[max_gas, avg_gas, temperature, humidity, R, G, B]`

The course project evaluated Polynomial Regression and Random Forest Regression. Random Forest was selected for deployment. The report records a 10-fold cross-validation mean R2 of approximately **0.92 ± 0.131**, and final-test metrics of **MAE 0.193**, **RMSE 0.324**, **R2 0.915**. These numbers should be interpreted in the context of the small proof-of-concept dataset.

## Actuation / UI
- **FruitMeter** servo gauge: predicted day maps to a 0-180 degree angle and is sent over MQTT.
- **FruitBot** Telegram notifications: status updates and spoilage warnings.
- **FruitApp** Flask dashboard: live sensor values, images and historical charts.

## Main runtime flow
1. ESP32 sensor node publishes temperature, humidity and gas readings.
2. ESP32-CAM publishes Base64 JPEG frames.
3. `data_collector.py` subscribes, decodes/logs data, runs color analysis and periodically creates `ml_input.json`.
4. `prediction.py` loads the trained Random Forest model/scaler and predicts Day 1-Day 5.
5. Prediction is published to the servo ESP32 and sent to Telegram.
6. Flask serves the monitoring dashboard.

## Known limitations
- MQ-2 measurements are sensitive to enclosure leakage.
- Manual banana placement changes sensor geometry.
- Battery-powered lighting causes brightness drift.
- The dataset is small and represents limited banana diversity.
