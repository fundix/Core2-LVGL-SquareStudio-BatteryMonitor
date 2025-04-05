# Core2 LVGL Battery Monitor

This project is a battery monitor application built for the M5Stack Core2 using LVGL (Light and Versatile Graphics Library) and PlatformIO. It displays the battery percentage and charging status with a modern touchscreen UI.

## Features

-   Display battery percentage on a progress bar
-   Show charging status with a visual icon
-   Auto-dim the display after inactivity (60s)
-   Touchscreen interaction support
-   Periodic UI updates using FreeRTOS timers
-   Audio feedback via M5 speaker
-   Built with LVGL v9 and M5Unified

## Hardware

-   M5Stack Core2 (or compatible with touchscreen and battery)

## Software Stack

-   [PlatformIO](https://platformio.org/)
-   [LVGL v9](https://lvgl.io/)
-   [M5Unified](https://github.com/m5stack/M5Unified)
-   ESP-IDF + Arduino core

## How to Build and Upload

1. Clone the repository
2. Open in VSCode with PlatformIO extension
3. Run `PlatformIO: Build`
4. Upload to the device with `PlatformIO: Upload`
5. Monitor the serial output using `PlatformIO: Monitor`

## File Overview

-   `src/main.cpp` — Main application logic, including LVGL setup, display dimming, and battery update handling
-   `ui/ui.h` — Auto-generated UI code (e.g., from SquareLine Studio)
-   `platformio.ini` — Project configuration for PlatformIO

## License

MIT License

## Author

[Your Name]
