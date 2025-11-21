# Web-Based ESP32 Thermal Imager (MLX90640)

Overview
--------

This repository contains an Arduino/PlatformIO-compatible sketch (`ThermIMG.ino`) for a web-based thermal camera using an ESP32 and a Melexis MLX90640 (32×24) IR sensor. The sketch reads temperature frames over I2C and serves a small web UI that fetches frames from the ESP32 and renders them on an HTML canvas.

This README explains wiring, how the sketch works, available HTTP endpoints, how to build and flash, and common troubleshooting notes.

Files
-----
- `ThermIMG.ino` — the Arduino-compatible sketch included in this repo. It:
  - Initializes I2C and the MLX90640 sensor
  - Connects to Wi‑Fi using hardcoded `ssid`/`password` constants
  - Serves a minimal web UI on `/`
  - Serves the latest frame as a comma-separated list of temperatures on `/frame`

Hardware
--------

Required components
- **ESP32** development board (ESP32-WROOM or similar)
- **MLX90640** 32×24 thermal camera module
- Jumper wires / breadboard (or a custom breakout)

Wiring (matches `ThermIMG.ino`)
- `MLX90640 VCC` -> `ESP32 3.3V`
- `MLX90640 GND` -> `ESP32 GND`
- `MLX90640 SDA` -> `ESP32 GPIO21 (SDA)`
- `MLX90640 SCL` -> `ESP32 GPIO22 (SCL)`

Notes
- Most MLX90640 breakout boards operate at 3.3V and are directly compatible with the ESP32. Verify your breakout's voltage requirements before powering.
- The sketch sets I2C clock to 1 MHz (`Wire.setClock(1000000)`); if you get communication errors, try lowering this (e.g., 400000 or 100000).

Sketch / Firmware overview
--------------------------

Main responsibilities in `ThermIMG.ino`:
- Initialize I2C with `Wire.begin(21, 22)`
- Configure the MLX90640 with `mlx.begin()`, set chess mode and refresh rate to 8 Hz
- Connect to Wi‑Fi using the `ssid` and `password` constants
- Host a `WebServer` on port 80
- Serve a static HTML page (`/`) and a frame endpoint (`/frame`)

Network endpoints (from the sketch)
- `GET /` — serves the single-file web UI (embedded as `MAIN_page`)
- `GET /frame` — returns the latest frame as a comma-separated plain-text list of 768 float values (32×24)

Web UI behavior (embedded in the sketch)
---------------------------------------

- The UI contains a small JavaScript renderer using a `<canvas id="c">` of 320×240 pixels.
- It polls `/frame` every 100 ms (via `setInterval(updateFrame, 100)`), parses the CSV of temperatures, finds min/max, maps temperatures to RGB colors with a simple linear mapping, and fills 10×10 blocks for each MLX90640 pixel (32×24 → 320×240 canvas).

Known issues & improvements
---------------------------

- Hardcoded Wi‑Fi credentials: `const char* ssid` and `password` are in the sketch. For security and convenience, implement captive portal / AP config or store credentials in non-volatile storage.
- Polling via HTTP every 100 ms may be inefficient. Consider using WebSocket streaming for lower latency and smaller payloads.
- The web UI uses a simple color mapping and computes min/max per-frame. For consistent color scaling across time, add a manual min/max control or shared calibrated range.
- Large text payloads: `/frame` returns 768 floats as CSV. Switching to binary frames (e.g., packed 16-bit values) or compressed frames will reduce latency and CPU overhead.
- The JS contains a redundant line in `getColor` (harmless but can be cleaned). See `ThermIMG.ino` for the embedded HTML.

How to build and flash
----------------------

Using PlatformIO
1. Create a `platformio.ini` for ESP32 or open the folder in PlatformIO.
2. Add required libraries (e.g., `Adafruit MLX90640` compatible library).
3. Build and upload:

```bash
pio run -t upload
```

Using Arduino IDE
1. Install the ESP32 board support and required MLX90640 library.
2. Open `ThermIMG.ino` in the Arduino IDE.
3. Select the correct ESP32 board and COM port, then click Upload.

First run
---------

1. Power the ESP32 with the MLX90640 wired as above.
2. Open the serial monitor at 115200 baud; the sketch prints connection progress and assigned IP address.
3. Open a browser to `http://<device-ip>/` shown in serial output.

Usage tips
----------
- If the UI is black or shows no data, check serial logs for Wi‑Fi and sensor initialization messages.
- If you see I2C errors, try lowering the I2C clock rate from 1 MHz.
- To reduce network load, increase the `setInterval` delay in the embedded UI or implement WebSocket streaming in the firmware.

Troubleshooting
---------------

- MLX90640 missing on startup:
  - Confirm wiring and that the sensor is powered at 3.3V.
  - Try lowering I2C clock or check for address conflicts.
- Wi‑Fi never connects:
  - Verify `ssid`/`password` and that the AP accepts the device.
  - Confirm the device is on the same subnet as your browser.
- Canvas shows uniform colors or strange mapping:
  - Check that `/frame` returns 768 values (32×24); open `http://<device-ip>/frame` to inspect raw output.

Next steps (suggested)
----------------------
- Replace HTTP polling with WebSocket streaming for lower-latency updates.
- Add a small settings page to change Wi‑Fi credentials and MLX90640 refresh rate without reflashing.
- Provide a PlatformIO example `platformio.ini` and a `lib_deps` list for easy reproduction.
- Add a README section that explains how to calibrate temperature mapping and palettes.

License
-------

This repository does not include a license by default. If you want to publish this project, consider adding an `LICENSE` file (for example, `MIT`).

Credits
-------

- Based on the Adafruit / SparkFun MLX90640 libraries and example usage patterns.
# Web_Base_Thermal_Imaging_MLX90640