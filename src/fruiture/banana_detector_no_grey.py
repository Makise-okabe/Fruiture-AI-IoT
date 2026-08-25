import cv2
import numpy as np

def detect_banana_ultimate(img):
    vis = img.copy()
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    hsv = cv2.GaussianBlur(hsv, (7, 7), 0)

    # Adaptive brightness normalization
    h, s, v = cv2.split(hsv)
    clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8))
    v_eq = clahe.apply(v)
    hsv = cv2.merge([h, s, v_eq])

    # Color ranges calibrated for ESP32-CAM lighting
    lower_green  = np.array([30, 40, 40])
    upper_green  = np.array([80, 255, 255])
    lower_yellow = np.array([18, 60, 60])
    upper_yellow = np.array([35, 255, 255])
    lower_brown  = np.array([5, 50, 50])
    upper_brown  = np.array([25, 200, 200])
    lower_black  = np.array([0, 0, 20])
    upper_black  = np.array([25, 120, 80])

    mask_green  = cv2.inRange(hsv, lower_green, upper_green)
    mask_yellow = cv2.inRange(hsv, lower_yellow, upper_yellow)
    mask_brown  = cv2.inRange(hsv, lower_brown, upper_brown)
    mask_black  = cv2.inRange(hsv, lower_black, upper_black)

    mask_total = cv2.bitwise_or(mask_yellow,
                    cv2.bitwise_or(mask_green,
                        cv2.bitwise_or(mask_brown, mask_black)))

    # Reject gray / low-saturation background regions
    gray_reject = cv2.inRange(hsv, (0, 0, 0), (180, 40, 255))
    mask_total = cv2.bitwise_and(mask_total, cv2.bitwise_not(gray_reject))

    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (7, 7))
    mask_total = cv2.morphologyEx(mask_total, cv2.MORPH_CLOSE, kernel, iterations=2)
    mask_total = cv2.morphologyEx(mask_total, cv2.MORPH_OPEN, kernel, iterations=2)

    mask_smooth = cv2.GaussianBlur(mask_total, (9, 9), 0)
    _, mask_smooth = cv2.threshold(mask_smooth, 127, 255, cv2.THRESH_BINARY)

    contours, _ = cv2.findContours(mask_smooth, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    if not contours:
        return vis, None, None, None

    banana_cnt = max(contours, key=cv2.contourArea)
    if cv2.contourArea(banana_cnt) < 400:
        return vis, None, None, None

    epsilon = 0.003 * cv2.arcLength(banana_cnt, True)
    banana_cnt = cv2.approxPolyDP(banana_cnt, epsilon, True)

    banana_mask = np.zeros(mask_total.shape, dtype=np.uint8)
    cv2.drawContours(banana_mask, [banana_cnt], -1, 255, -1)

    masked_pixels = img[banana_mask == 255]
    if masked_pixels.size == 0:
        return vis, None, None, None

    mean_rgb = tuple(int(np.median(masked_pixels[:, i])) for i in [2, 1, 0])

    total_px = cv2.countNonZero(mask_total)
    if total_px == 0:
        return vis, mean_rgb, None, None

    color_counts = {
        "green":  cv2.countNonZero(cv2.bitwise_and(mask_green, banana_mask)),
        "yellow": cv2.countNonZero(cv2.bitwise_and(mask_yellow, banana_mask)),
        "brown":  cv2.countNonZero(cv2.bitwise_and(mask_brown, banana_mask)),
        "black":  cv2.countNonZero(cv2.bitwise_and(mask_black, banana_mask))
    }
    proportions = {k: round(v / total_px * 100, 1) for k, v in color_counts.items()}

    ripeness = int(round(100 * (
        0.0 * proportions.get("green", 0) / 100 +
        0.3 * proportions.get("yellow", 0) / 100 +
        0.7 * proportions.get("brown", 0) / 100 +
        1.0 * proportions.get("black", 0) / 100
    )))

    x, y, w, h = cv2.boundingRect(banana_cnt)
    cv2.rectangle(vis, (x, y), (x + w, y + h), (255, 0, 0), 2)
    cv2.drawContours(vis, [banana_cnt], -1, (0, 220, 0), 3)

    label = f"Ripeness: {ripeness}/100 | RGB {mean_rgb}"
    cv2.putText(vis, label, (x, y - 10),
                cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 0), 2, cv2.LINE_AA)

    return vis, mean_rgb, ripeness, proportions
