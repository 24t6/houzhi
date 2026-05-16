# Project Structure

This STM32F407 project is organized into a small set of top-level areas so the codebase is easier to navigate and maintain.

## Top-level directories

- `APP`
  - Application entry and feature orchestration code.
  - `src/` contains application source files.
  - `inc/` contains application headers.
- `BSP`
  - STM32F4 chip support, system clock, interrupt, and project configuration headers.
  - `src/` contains system and interrupt source files.
  - `inc/` contains chip support and configuration headers.
- `USER`
  - Keil project files plus chip/startup support files tied to the project template.
- `SYSTEM`
  - Shared low-level system utilities such as `delay` and `sys`.
- `STM32`
  - Board-specific modules grouped by responsibility.
- `tools`
  - One-off maintenance scripts used during development.
- `CORE`
  - CMSIS core startup assembly and ARM core headers.
- `FWLIB`
  - STM32 Standard Peripheral Library.
- `OBJ`
  - Generated build artifacts from Keil.

## STM32 module layout

- `STM32/drivers/uart`
  - UART and USART drivers for `usart1` to `usart6`.
- `STM32/sensors`
  - Sensor and radar-facing modules: `HX711`, `HC-SR04`, `gy95t`, `Radar`.
- `STM32/display`
  - Display-related module: `LatticeLed`.
- `STM32/comm`
  - External communication modules such as `4GTX`.
- `STM32/network`
  - Board-side ESP8266 support and Wi-Fi/MQTT support helpers.
- `STM32/timer`
  - Timer-related helpers such as `GPTM0`.
- `APP/src` and `APP/inc`
  - Application orchestration modules, including `app_esp8266`.

## Current architecture notes

- `APP/src/main.c` uses a bare-metal polling loop instead of an RTOS scheduler.
- `BSP` now holds the STM32 support headers and interrupt/system source files that previously cluttered `USER`.
- Application orchestration has already been split into `APP` modules:
  - `app_init.c`
  - `app_sensor_fusion.c`
  - `app_alarm.c`
  - `app_upload.c`
  - `app_context.c`
- The main loop calls those application services in sequence and uses `delay_ms()` to pace the cycle.

## What was intentionally not changed

- `FWLIB` and `CORE` were left in place to avoid unnecessary vendor-library risk.
- `OBJ` was kept as-is because it is generated output and tied to the current Keil build flow.
- Existing include names such as `#include "Radar.h"` were preserved by updating Keil include paths instead of rewriting every source file.
- Keil source groups were split by responsibility so application, communication, sensor, display, UART, and timer files are easier to locate.
