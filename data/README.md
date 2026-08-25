# Data

The original project used synchronized environmental, gas and visual features collected across banana ripening stages Day 1–5.

For portability, the original Excel training workbook has been exported to:

- `src/fruiture/dataset3_app.csv`

The model features are:

`Max_gas, Average_Gas, temperature, humidity, R, G, B`

The target is `day` (1–5).

The development archive also contained raw sensor logs and hundreds of camera frames. Those large binary/image archives are not duplicated in this public portfolio repository; the repository focuses on the reproducible code, firmware and model-training dataset.
