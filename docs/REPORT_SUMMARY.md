# Fruiture Project Report Summary

## Project

**Fruiture: An Integrated AI-IoT System for Real-Time Detection and Prediction of Fruit Ripeness Using Multi-Sensor Data Fusion**

Team: Du Yanzhang, Fang Tianchi, Feng Yilong, Liu Hengyi  
Report date: 20 November 2025

## Overview

Fruiture is a low-cost AI-IoT prototype designed to monitor and predict banana ripeness using environmental, chemical and visual signals. The system integrates embedded sensing, MQTT communication, computer vision, machine learning, physical actuation, Telegram notifications and a Flask dashboard into one end-to-end workflow.

## Hardware and sensing

The sensing platform combines:

- **DHT11** for ambient temperature and relative humidity.
- **MQ-2** gas sensor as a VOC-related ripening/spoilage signal.
- **ESP32-CAM / OV2640** for banana-skin colour information.
- **ESP32** microcontrollers for sensing, networking and servo actuation.

The report describes calibration and signal-stability measures including DHT11 comparison against a reference hygrometer, MQ-2 warm-up/baseline calibration and averaging, fixed camera illumination, synchronized timestamped measurements, and a 470 µF capacitor across the supply rails to reduce voltage dips from the MQ-2 heater load.

## Vision processing

The camera pipeline uses OpenCV rather than a deep neural network. Images are converted from BGR to HSV, brightness-normalized with CLAHE, and segmented into green, yellow, brown and black regions. Low-saturation background areas are rejected, morphology and contour extraction isolate the banana, and RGB statistics plus colour proportions are generated as numerical features.

## Data collection and feature engineering

Data were collected in two batches of five bananas. The experimental window was reduced to five days after preliminary observations showed that the bananas had become fully rotten by Day 5.

The final machine-learning feature set was:

`[max_gas, avg_gas, temperature, humidity, R, G, B]`

Mutual Information analysis indicated that maximum/average gas features were the most informative, with RGB—especially the green channel—also contributing strongly. Humidity showed moderate relevance and temperature had the lowest MI score in the controlled setup.

## Machine learning

The project formulated ripeness estimation as a regression problem and compared:

- Random Forest Regression
- Polynomial Regression (degree 3)

The Random Forest used 200 trees and was selected for deployment because it was more stable and better suited to the nonlinear, noisy, small dataset.

### Reported Random Forest performance

| Metric | Result |
|---|---:|
| 10-fold cross-validation mean R² | **0.92 ± 0.131** |
| Final test MAE | **0.193** |
| Final test RMSE | **0.324** |
| Final test R² | **0.915** |

Polynomial Regression showed substantial instability and overfitting in cross-validation and was not selected for deployment.

## End-to-end system flow

1. DHT11, MQ-2 and ESP32-CAM collect environmental, chemical and visual data.
2. ESP32 devices publish measurements over Wi-Fi to MQTT topics in the `fruiture/` namespace.
3. `data_collector.py` subscribes to MQTT, timestamps and logs readings, reconstructs Base64 camera images, runs OpenCV colour analysis, and generates machine-learning input summaries.
4. `prediction.py` loads the trained Random Forest model and predicts a Day 1–5 ripeness stage.
5. The prediction is mapped to a servo angle and published via MQTT to the FruitMeter.
6. A Telegram bot sends remote ripeness updates and spoilage warnings.
7. A Flask dashboard displays live sensor values, historical logs, images and model outputs.

## User interfaces

### FruitMeter
A servo-driven physical gauge maps predicted Day 1–5 ripeness to an angular pointer position, providing an immediate local indication of fruit condition.

### FruitBot
A Telegram notification system sends context-specific ripeness messages and warns the user as the banana approaches spoilage.

### FruitApp
The Flask dashboard provides remote visualization of gas concentration, temperature, humidity, images and historical readings.

## Live demonstration

During the reported live demo, a Day-4 banana was placed in the system. The sensors collected data, the model predicted Day 4, the FruitMeter moved to 4, the Telegram bot sent the ripeness status, and the dashboard displayed the live measurements.

## Limitations identified in the report

- Gas readings are sensitive to leakage from the enclosure.
- Manual banana placement changes distance/orientation relative to sensors.
- Battery voltage decline can change LED brightness and therefore RGB measurements.
- The dataset contains only a limited number and variety of bananas, restricting generalization.

## Future work

The report proposes larger and more diverse datasets, continuous/periodic model retraining, extension to fruits such as apples, avocados and mangoes, and cloud-based storage/analytics for longer-term and multi-location monitoring.
