# ESP8266 Clock & Weather Informer

A feature-rich weather informer based on ESP8266 with LED matrix display, DHT22 sensor, and Telegram bot integration.

![PlatformIO](https://img.shields.io/badge/PlatformIO-espressif8266-blue)
![License](https://img.shields.io/badge/license-MIT-green)

## Features

- **LED Matrix Display**: 72x8 pixel display using 4x MAX7219 modules with Russian font support
- **Time & Date**: Synchronized via NTP with configurable timezone offset
- **Indoor Sensors**: DHT22 for real-time temperature and humidity monitoring
- **Outdoor Weather**: OpenWeatherMap API integration for weather conditions
- **Telegram Bot**: Remote control and custom message display via Telegram
- **OTA Updates**: Over-the-air firmware updates via WiFi
- **Non-Blocking Architecture**: Responsive operation with state machine pattern

## Hardware

| Component | Specification |
|-----------|----------------|
| Microcontroller | ESP8266 (WeMos D1 Mini) |
| Display | 4x MAX7219 modules (72x8 pixels) |
| Indoor Sensor | DHT22 temperature & humidity |
| Buzzer | Passive buzzer (GPIO 5) |
| Connectivity | WiFi 2.4GHz |

## Pin Configuration

| Pin | Function |
|------|----------|
| GPIO 14 | SPI Clock (CLK) |
| GPIO 13 | SPI Data (MOSI) |
| GPIO 12 | SPI Chip Select (CS) |
| GPIO 5 | Buzzer |
| GPIO 2 | DHT22 Data |

## Installation

### Prerequisites

- PlatformIO installed
- ESP8266 development board

### Setup

1. Clone the repository:
```bash
git clone https://github.com/planer-pro/Clock_weather_telegram_informer.git
cd Clock_weather_telegram_informer
```

2. Create configuration file:
```bash
cp include/config.example.h include/config.h
```

3. Edit `include/config.h` with your credentials:
```cpp
// WiFi
constexpr const char* WIFI_SSID = "Your_WiFi_SSID";
constexpr const char* WIFI_PASSWORD = "Your_WiFi_Password";

// Telegram Bot (get token from @BotFather)
constexpr const char* TELEGRAM_BOT_TOKEN = "1234567890:ABC...";

// OpenWeatherMap (get key from https://openweathermap.org/api)
constexpr const char* OPENWEATHER_API_KEY = "your_api_key";
constexpr float LOCATION_LATITUDE = 55.7558;  // Example: Moscow
constexpr float LOCATION_LONGITUDE = 37.6173;
```

4. Build and upload:
```bash
pio run -t upload
```

For OTA upload (when device is online):
```bash
pio run -t upload -e upload_port=192.168.x.x
```

## Telegram Bot Commands

| Command | Description |
|----------|-------------|
| `.help` | List all available commands |
| `.DBR 0-15` | Set display brightness |
| `.TEMP` | Show indoor temperature & humidity |
| `.WEATHER` | Show outdoor weather |
| `.STATUS` | Device status (memory, WiFi, RSSI) |
| `.TIME` | Force time display |
| `<text>` | Display custom message on matrix |

## Architecture

The project uses modular architecture with state machine pattern:

```
src/
├── main.cpp              # Entry point with main state machine
├── network.cpp/.h        # WiFi, NTP, OTA
├── display.cpp/.h         # MAX7219 display manager
├── sensors.cpp/.h         # DHT22 sensor manager
├── weather.cpp/.h         # OpenWeatherMap API
├── telegram_bot.cpp/.h    # Telegram bot & commands
├── async_beep.cpp/.h     # Non-blocking buzzer
└── utils.cpp/.h          # Helper functions
```

### Non-Blocking Design

All time-sensitive operations use `millis()` timers instead of `delay()`:
- Display animations continue while processing Telegram messages
- WiFi connection doesn't block other operations
- Buzzer plays asynchronously

## Dependencies

| Library | Version | Purpose |
|----------|----------|---------|
| MD_Parola | 3.7.5 | LED matrix display control |
| DHT sensor library | 1.4.7 | DHT22 temperature/humidity |
| ArduinoJson | 6.21.6 | JSON parsing |
| CTBot | 2.1.14 | Telegram Bot API |
| Adafruit Unified Sensor | 1.1.15 | Sensor interface |

## Configuration Options

Located in `include/constants.h`:

- `TELEGRAM_POLL_PERIOD`: Telegram message poll interval (default: 500ms)
- `NTP_UPDATE_PERIOD`: Time sync interval (default: 30s)
- `DHT_READ_PERIOD`: Sensor read interval (default: 10s)
- `WEATHER_UPDATE_PERIOD`: Weather refresh interval (default: 10min)
- `DEFAULT_DISPLAY_INTENSITY`: Initial brightness 0-15
- `BUF_SIZE`: Display text buffer size (default: 64)

## Known Limitations

1. **ArduinoJson 6.x Required**: CTBot library is incompatible with ArduinoJson 7.x
2. **Blocking Weather Updates**: OpenWeatherMap API call blocks for 2-5 seconds (executes every 10min)
3. **Memory Usage**: ~49% RAM used (40KB of 81KB available)

## Building

```bash
pio run              # Build firmware
pio run -t upload   # Upload via USB
pio run -t uploadfs # Upload filesystem
```

## License

This project is licensed under the MIT License.

## Author

Clock Informer Team

## Credits

- [MD_Parola](https://github.com/MajicDesigns/MD_Parola) - LED matrix library
- [CTBot](https://github.com/shurillu/CTBot) - Telegram Bot library
- [ESP8266 Weather Station](https://github.com/ThingPulse/esp8266-weather-station) - Weather API client
