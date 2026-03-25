# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESP8266-based weather informer with LED matrix display (MAX7219 4x8 modules, 72x8 pixels). Features:
- Time/date display via NTP
- Indoor temperature/humidity via DHT22
- Outdoor weather via OpenWeatherMap API
- Telegram bot for remote control and custom messages
- OTA updates over WiFi

## Build Commands

```bash
pio run              # Build firmware
pio run -t upload   # Upload via serial
pio run -t uploadfs # Upload filesystem (if needed)
pio run -t upload -e upload_port=192.168.x.x  # OTA upload
```

## Configuration

The project uses a two-file configuration system:

1. `include/config.example.h` - Template with placeholders (git-tracked)
2. `include/config.h` - Actual configuration (git-ignored)

**Before first build**, copy `config.example.h` to `config.h` and fill in:
- WiFi credentials (`WIFI_SSID`, `WIFI_PASSWORD`)
- Telegram bot token (`TELEGRAM_BOT_TOKEN`)
- OpenWeatherMap API key (`OPENWEATHER_API_KEY`)
- Location coordinates (`LOCATION_LATITUDE`, `LOCATION_LONGITUDE`)

**Do not modify `config.example.h`** - it's a template for new setups.

## Architecture

The codebase uses modular architecture with state machine pattern for non-blocking operation.

### Module Structure

| Module | File | Purpose |
|---------|-------|---------|
| Network | `src/network.cpp/.h` | WiFi connection, NTP time, OTA |
| Display | `src/display.cpp/.h` | MAX7219 LED matrix, animations |
| Sensors | `src/sensors.cpp/.h` | DHT22 temperature/humidity |
| Weather | `src/weather.cpp/.h` | OpenWeatherMap API |
| TelegramBot | `src/telegram_bot.cpp/.h` | Bot commands, message handling |
| AsyncBeep | `src/async_beep.cpp/.h` | Non-blocking buzzer |
| Utils | `src/utils.cpp/.h` | Helper functions |

### Non-Blocking Design

The project avoids `delay()` calls to maintain responsiveness:
- **Display**: Call `display.update()` once per `loop()` iteration. Returns true when animation completes.
- **Buzzer**: `AsyncBeep` class uses internal state machine with millis().
- **Main Loop**: `MainState` enum controls screen transitions without blocking.

**Important**: Never use `delay()` in new code. Use `millis()` timers or add to existing state machines.

### Telegram Command System

Commands are registered via `telegram.registerCommand()` in `initDefaultCommands()`:

```cpp
telegram.registerCommand(".CMD", [](const String& params, const TBMessage& msg) {
    // Handler logic
    return "Response";
}, "Description");
```

**Built-in commands**:
- `.help` - List all commands
- `.DBR 0-15` - Set display brightness
- `.TEMP` - Show indoor temperature
- `.WEATHER` - Show outdoor weather
- `.STATUS` - Device status (memory, WiFi, RSSI)
- `.TIME` - Force time display

### Constants Location

- **Hardware pins and timings**: `include/constants.h`
  - Modify only for hardware changes
  - `BUF_SIZE = 64` is optimized for 4 modules (32 chars max)
- **Configuration**: `include/config.h`
  - Credentials, API keys, location
  - Modify per installation

### Russian Font

The project uses `src/6bite_rus.h` as custom Russian font for MD_Parola.
- Each row is 8 bytes (8x8 pixel block)
- Characters indexed by ASCII code

## Known Limitations

1. **ArduinoJson version**: Must stay at 6.x (not 7.x) due to CTBot library incompatibility.
2. **Weather updates**: `WeatherManager::update()` uses blocking HTTP call (library limitation). Called every 10 minutes only.
3. **Memory**: ~49% RAM used (40KB of 81KB). Adding large buffers may require optimization.

## Dependencies

- Platform: espressif8266@4.2.1
- Board: d1_mini (ESP8266)
- Key libraries: MD_Parola 3.7.5, DHT 1.4.7, CTBot 2.1.14, ArduinoJson 6.21.5

Local library: `lib/esp8266-weather-station` for OpenWeatherMap API.
