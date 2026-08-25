# -*- coding: utf-8 -*-
"""Train the Fruiture Random Forest ripeness model from the portable CSV dataset."""

import os
import pandas as pd
import numpy as np
import joblib
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.preprocessing import StandardScaler
from sklearn.ensemble import RandomForestRegressor
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DATASET = os.path.join(BASE_DIR, "dataset3_app.csv")
MODEL_FILE = os.path.join(BASE_DIR, "banana_regression_model.pkl")
SCALER_FILE = os.path.join(BASE_DIR, "banana_scaler.pkl")

# Load the project dataset exported from the original workbook.
df = pd.read_csv(DATASET)

# Reproduce the project's robustness strategy: add noise to temperature/humidity
# so the model relies less heavily on environment-specific readings.
rng = np.random.default_rng(42)
df["temperature"] += rng.normal(0, 10.0, len(df))
df["humidity"] += rng.normal(0, 25.0, len(df))
df["temperature"] = df["temperature"].clip(15, 40)
df["humidity"] = df["humidity"].clip(10, 90)

FEATURES = ["Max_gas", "Average_Gas", "temperature", "humidity", "R", "G", "B"]
X = df[FEATURES]
y = df["day"]

X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42
)

scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)

rf = RandomForestRegressor(n_estimators=200, random_state=42)
rf.fit(X_train_scaled, y_train)

y_pred = rf.predict(X_test_scaled)
mae = mean_absolute_error(y_test, y_pred)
rmse = np.sqrt(mean_squared_error(y_test, y_pred))
r2 = r2_score(y_test, y_pred)

print("=== Fruiture Random Forest Results ===")
print(f"MAE:  {mae:.3f}")
print(f"RMSE: {rmse:.3f}")
print(f"R²:   {r2:.3f}")

# Report a CV estimate for reproducibility. The exact score may differ from the
# course report because this script fixes the random noise seed.
cv_scores = cross_val_score(rf, scaler.fit_transform(X), y, cv=10, scoring="r2")
print(f"10-fold CV R²: {cv_scores.mean():.3f} ± {cv_scores.std():.3f}")

joblib.dump(rf, MODEL_FILE)
joblib.dump(scaler, SCALER_FILE)
print(f"Saved model:  {MODEL_FILE}")
print(f"Saved scaler: {SCALER_FILE}")
