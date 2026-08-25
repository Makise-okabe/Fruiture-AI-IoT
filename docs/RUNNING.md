# Running Fruiture

## 1. Python environment

```bash
python -m venv .venv
# Windows: .venv\Scripts\activate
# macOS/Linux: source .venv/bin/activate
pip install -r requirements.txt
```

## 2. Train the model

The original Excel training workbook has been exported to a portable CSV in the repository. Train the Random Forest model before running inference:

```bash
cd src/fruiture
python model_regression.py
```

This creates `banana_regression_model.pkl` and `banana_scaler.pkl` locally.

## 3. Telegram configuration (optional)

Copy:

```text
BotAPI.example.txt -> BotAPI.txt
```

Then replace the placeholders with a newly generated Telegram bot token and your chat ID. `BotAPI.txt` is git-ignored and must never be committed.

## 4. Firmware

Open the appropriate sketches in Arduino IDE and replace `YOUR_WIFI_SSID` / `YOUR_WIFI_PASSWORD` locally before flashing:

- `firmware/sensor_node/mqtt_dht11.ino`
- `firmware/camera_final/camara_final_sleep.ino`
- `firmware/servo/servo_1.ino`

## 5. Start the backend and dashboard

From `src/fruiture`:

```bash
python data_collector.py
```

The Flask dashboard runs at `http://localhost:5001` and the backend subscribes to the `fruiture/#` MQTT namespace.

## 6. Run prediction

After the collector generates `ml_input.json`:

```bash
python prediction.py
```

The predictor loads the locally generated model/scaler, predicts Day 1–5, publishes a servo angle via MQTT, and optionally sends a Telegram notification.
