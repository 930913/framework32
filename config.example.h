// Copy this file to config.h and edit with your configuration
// See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for timezone

char configJson[4096] = R"CONFIG({
  "sleepTimeout": 20000,
  "rotation": 0,
  "screenBrightness": 255,
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
  },
  "Octoprint": {
    "server": "https://192.168.0.1",
    "apikey": "???",
    "sleepDisabled": true,
    "refreshTime": 5000
  },
  "OTP Auth": {
    "base32Key": "???"
  }
})CONFIG";
