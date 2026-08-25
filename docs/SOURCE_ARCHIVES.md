# Source Consolidation

This public repository was consolidated from the team's original project files, including the final Python/ML application archive, ESP32-CAM development archives, technical report, presentation materials and hardware-related project files.

The public GitHub version prioritizes readable and reproducible source material:

- Final Python MQTT / OpenCV / Flask / ML code
- Final ESP32 sensor, camera and servo firmware
- Portable CSV export of the training workbook
- Technical documentation derived from the final report
- Security-sanitized configuration examples

Large binary development artifacts (full camera-frame archives, original PPTX/PDF binaries, CAD/fabrication binaries and serialized model files) are intentionally not committed through this public source tree. The trained model can be regenerated from the included CSV with `model_regression.py`.

No live Wi-Fi or Telegram credentials are included.
