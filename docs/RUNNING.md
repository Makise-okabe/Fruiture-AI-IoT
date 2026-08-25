# Running Fruiture

## 1. Python environment

```bash
python -m venv .venv
# Windows: .venv\Scripts\activate
# macOS/Linux: source .venv/bin/activate
pip install -r requirements.txt
```

## 2. Telegram configuration (optional)
Copy:

```text
src/fruiture/BotAPI.example.txt -> src/fruiture/BotAPI.txt
```

Then replace the placeholders with a newly created bot token and chat ID. `BotAPI.txt` is git-ignored.

## 3. Firmware
Open the appropriate sketches in Arduino IDE and replace `YOUR_WIFI_SSID` / `YOUR_WIFI_PASSWORD` locally before flashing. Do not commit real credentials.

Recommended final sketches:
- `firmware/sensor_node/mqtt_dht11.ino`
- `firmware/camera_final/camara_final_sleep.ino`
- `firmware/servo/servo_1.ino`

## 4. Start backend/dashboard

```bash
cd src/fruiture
python data_collector.py
```

The Flask dashboard runs at `http://localhost:5001`.

## 5. Run prediction
Once `ml_input.json` has been generated:

```bash
python prediction.py
```

This loads the included model/scaler, publishes the servo command and (when configured) sends a Telegram notification.

## 6. Retrain model

```bash
python model_regression.py
```

The training script expects `dataset3 app.xlsx` in its working directory and writes the model/scaler files locally.
