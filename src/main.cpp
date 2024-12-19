#include <Arduino.h>
#include <MD_Parola.h>
#include <6bite_rus.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <CTBot.h>
#include <NTPClient.h>
// #include <WiFiUdp.h>
#include <DHT.h>
// #include <JsonListener.h>
// #include <time.h>
#include <OpenWeatherMapOneCall.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN 14
#define DATA_PIN 13
#define CS_PIN 12
#define BEEP_PIN 5
#define DHTPIN 2

#define TELEG_PERIOD 500 // 1/2 sec
#define NTP_PERIOD 30000 // 1/2 min
#define DHT_PERIOD 10000 // 10 sec
// #define WEATHER_PERIOD 1800000 // 30 min
#define WEATHER_PERIOD 600000 // 10 min
#define BEEP_PERIOD 80
#define BEEP_COUNT 2

#define BOTtoken "bot_token_here" //@hata2bot 46 Chars
#define BUF_SIZE 512

struct sCatalog
{
  textEffect_t effect;
  const char *psz;
  uint16_t speed;
  uint16_t pause;
};

sCatalog catalog[] =
    {
        {PA_PRINT, "", 40, 3000},
        {PA_SCROLL_UP, "", 40, 3000},
        {PA_SCROLL_DOWN, "", 40, 3000},
        {PA_SCROLL_LEFT, "", 40, 3000},
        {PA_SCROLL_RIGHT, "", 40, 3000},
        {PA_SPRITE, "", 40, 3000},
        {PA_SLICE, "", 8, 3000},
        {PA_MESH, "", 80, 3000},
        {PA_FADE, "", 125, 3000},
        {PA_DISSOLVE, "", 250, 3000},
        {PA_BLINDS, "", 60, 3000},
        {PA_RANDOM, "", 25, 3000},
        {PA_WIPE, "", 40, 3000},
        {PA_WIPE_CURSOR, "", 40, 3000},
        {PA_SCAN_HORIZ, "", 40, 3000},
        {PA_SCAN_HORIZX, "", 40, 3000},
        {PA_SCAN_VERT, "", 40, 3000},
        {PA_SCAN_VERTX, "", 40, 3000},
        {PA_OPENING, "", 40, 3000},
        {PA_OPENING_CURSOR, "", 40, 3000},
        {PA_CLOSING, "", 40, 3000},
        {PA_CLOSING_CURSOR, "", 40, 3000},
        {PA_SCROLL_UP_LEFT, "", 40, 3000},
        {PA_SCROLL_UP_RIGHT, "", 40, 3000},
        {PA_SCROLL_DOWN_LEFT, "", 40, 3000},
        {PA_SCROLL_DOWN_RIGHT, "", 40, 3000},
        {PA_GROW_UP, "", 40, 3000},
        {PA_GROW_DOWN, "", 40, 3000},
};

String OPEN_WEATHER_MAP_APP_ID = "3d653a91e0205d59d2e8a701f0b0db39"; // 32 Chars
String OPEN_WEATHER_MAP_LANGUAGE = "ru";
float OPEN_WEATHER_MAP_LOCATTION_LAT = 53.972875; // 9 Chars
float OPEN_WEATHER_MAP_LOCATTION_LON = 27.830926;
boolean IS_METRIC = true;

uint32_t _tmTelergamReq, _tmNtp, _tmDht, _tmWeather;
uint8_t mainStage;

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
CTBot myBot;
DHT dht(DHTPIN, DHT22);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
OpenWeatherMapOneCallData openWeatherMapOneCallData;

char ssid[] = "ssid_here"; // 20 Chars
char pass[] = "pass_here"; // 20 Chars

char curDisplayMessage[BUF_SIZE] = {"Home Technologies"}; // 17 Chars
String weekCatalg[] = {" Воскресенье", " Понедельник", " Вторник", " Среда", " Четверг", " Пятница", " Суббота"};
String monthCatalog[] = {"января", "февраля", "марта", "апреля", "мая", "июня", "июля", "августа", "сентября", "октября", "ноября", "декабря"};
String timeLine;
String dateLine;
String dhtLine;
String weatherLine;

void getTMessageToVars();
void playBeep();
void getTimeToVars();
void getDhtData();
void getWeatherToVars();

void setup()
{
  Serial.begin(115200);

  pinMode(BEEP_PIN, OUTPUT);

  randomSeed(analogRead(A0));

  Serial.println("\nConnecting");

  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(BOTtoken);

  if (myBot.testConnection())
    Serial.println("\ntestConnection OK");
  else
    Serial.println("\ntestConnection NOK");

  ArduinoOTA.begin();

  timeClient.begin();
  timeClient.setTimeOffset(10800);

  dht.begin();

  P.begin();
  P.setInvert(false);
  P.setIntensity(0);
  P.setFont(0, _6bite_rus);

  _tmTelergamReq = millis() + TELEG_PERIOD; // quick start whithout delay
  _tmNtp = millis() + NTP_PERIOD;
  _tmDht = millis() + DHT_PERIOD;
  _tmWeather = millis() + WEATHER_PERIOD;

  P.displayText(curDisplayMessage, PA_CENTER, 25, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  while (!P.displayAnimate())
    ESP.wdtFeed();
}

void loop()
{
  // getTimeToVars();
  // getWeatherToVars();
  // getTMessageToVars();
  // playBeep();

  ArduinoOTA.handle();

  getTMessageToVars();

  static uint8_t rnd;
  static uint8_t _dobav;

  switch (mainStage)
  {
  case 0:
    getTimeToVars();

    timeLine.toCharArray(curDisplayMessage, BUF_SIZE);
    rnd = random(6, 27);

    P.displayText(curDisplayMessage, PA_CENTER, catalog[rnd].speed, 4000, catalog[rnd].effect, catalog[rnd].effect);
    while (!P.displayAnimate())
      ESP.wdtFeed();

    if (_dobav == 3)
      _dobav = 0;

    mainStage = ++_dobav;
    break;

  case 1:
    dateLine.toCharArray(curDisplayMessage, BUF_SIZE);
    P.displayText(curDisplayMessage, PA_CENTER, 25, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    while (!P.displayAnimate())
      ESP.wdtFeed();

    mainStage = 0;
    break;

  case 2:
    getDhtData();

    dhtLine.toCharArray(curDisplayMessage, BUF_SIZE);
    P.displayText(curDisplayMessage, PA_CENTER, 25, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    while (!P.displayAnimate())
      ESP.wdtFeed();

    mainStage = 0;
    break;

  case 3:
    getWeatherToVars();

    weatherLine.toCharArray(curDisplayMessage, BUF_SIZE);
    P.displayText(curDisplayMessage, PA_CENTER, 25, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    while (!P.displayAnimate())
      ESP.wdtFeed();

    mainStage = 0;
    break;

  case 4:
    P.displayText(curDisplayMessage, PA_CENTER, 25, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    while (!P.displayAnimate())
      ESP.wdtFeed();

    mainStage = 0;
    break;
  }
}

void getTimeToVars()
{
  if (millis() - _tmNtp > NTP_PERIOD)
  {
    timeClient.forceUpdate();

    uint8_t hr, mn;

    hr = timeClient.getHours();
    mn = timeClient.getMinutes();

    timeClient.getFormattedTime();

    String dw = weekCatalg[timeClient.getDay()];

    String formattedDate = timeClient.getFormattedDate();
    int splitT = formattedDate.indexOf("T");
    String data = formattedDate.substring(0, splitT);

    String yr, ms, dy;

    for (size_t i = 0; i < data.length(); i++)
    {
      if (i == constrain(i, 0, 3))
        yr += data[i];
      else if (i == constrain(i, 5, 6))
        ms += data[i];
      else if (i == constrain(i, 8, data.length()))
        dy += data[i];
    }

    timeLine = hr < 10 ? "0" + (String)hr + ":" : (String)hr + ":";
    timeLine += mn < 10 ? "0" + (String)mn : (String)mn;
    dateLine = dw + " " + (String)dy.toInt() + " " + monthCatalog[ms.toInt() - 1] + " " + yr + " года";

    _tmNtp = millis();
  }
}

void getDhtData()
{
  if (millis() - _tmDht > DHT_PERIOD)
  {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    String st = (String)t;
    String sh = (String)h;

    st.remove(st.length() - 1);
    sh.remove(sh.length() - 1);

    dhtLine = " Температура в доме: " + st + "C, " + "Влажность: " + sh + "%";

    _tmDht = millis();
  }
}

void getWeatherToVars()
{
  if (millis() - _tmWeather > WEATHER_PERIOD)
  {
    OpenWeatherMapOneCall *oneCallClient = new OpenWeatherMapOneCall();

    oneCallClient->setMetric(IS_METRIC);
    oneCallClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
    oneCallClient->update(&openWeatherMapOneCallData, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATTION_LAT, OPEN_WEATHER_MAP_LOCATTION_LON);

    delete oneCallClient;
    oneCallClient = nullptr;

    weatherLine = " На улице: ";
    weatherLine += String(openWeatherMapOneCallData.current.temp, 1) + "C, " + openWeatherMapOneCallData.current.weatherDescription;

    _tmWeather = millis();
  }
}

void getTMessageToVars()
{
  if (millis() - _tmTelergamReq > TELEG_PERIOD)
  {
    TBMessage msg;

    if (CTBotMessageText == myBot.getNewMessage(msg))
    {
      String s = msg.text;

      if (s[0] == '.')
      {
        if (s[1] == 'D' && s[2] == 'B' && s[3] == 'R')
        {
          s.remove(0, 4);

          for (size_t i = 0; i < s.length(); i++)
          {
            if (!isDigit(s[i]))
              myBot.sendMessage(msg.sender.id, "Не верный параметр. От 0-15");
          }
          int n = s.toInt();

          if (n < 0 || n > 15)
            myBot.sendMessage(msg.sender.id, "Параметр выходит за пределы. От 0-15");
          else
          {
            myBot.sendMessage(msg.sender.id, "Команда выполнена");
            P.setIntensity(n);
          }
        }
        else
          myBot.sendMessage(msg.sender.id, "Не описанная команда");
      }
      else
      {
        myBot.sendMessage(msg.sender.id, "Сообщение принято");
        playBeep();
        msg.text.toCharArray(curDisplayMessage, BUF_SIZE);

        mainStage = 4;
      }
    }

    _tmTelergamReq = millis();
  }
}

void playBeep()
{
  for (size_t i = 0; i < (BEEP_COUNT * 2); i++)
  {
    digitalWrite(BEEP_PIN, !digitalRead(BEEP_PIN));
    if (i != (BEEP_COUNT * 2) - 1)
      delay(BEEP_PERIOD);
  }
}