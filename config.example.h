// Copy this file to config.h and edit with your configuration

char configJson[4096] = R"CONFIG({
  "sleepTimeout": 20000,
  "rotation": 0,
  "screenBrightness": 255,
  // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for timezone
  "timezone": "GMT0BST,M3.5.0/1,M10.5.0",
  "clockFormat": "%H:%M",
  "wifi": [
    {
      "ssid": "???",
      "password": "???"
    },
    {
      "ssid": "???",
      "password": "???"
    }
  ],
  "Hive": {
    "username": "???",
    "password": "???"
  }
})CONFIG";
