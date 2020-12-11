# Framework32

Framework32 is designed to give your ESP32 (currently TTGO T-Display) the ability
to have multiple, graphical apps, controlled by an intuitive navigational system.

## Features

Current feature are:
- Load multiple applications onto your device and choose what to run at runtime
- Graphical "app-style" menu system with infinite scrolling
- Navigate "home" from anywhere by double clicking the left button
- Status bar
  - Time (from NTP)
  - WiFi strength indicator
  - Battery level indicator
- Sleep if not being used (but can be overridden by the open app)
- Config file (with the plan to make it dynamically updateable)
- Easy button handlers provided


## How to use

- Double click left button to return to the menu
- Left click - Go left/up
- Right click - Go right/down
- Long right click - Select

## Getting started

1. Copy `config.example.h` to `config.h`
2. Populate `config.h` with your config (e.g. WiFi connections).

## How to create an app

Take a look at the [DemoApp](DemoApp.h) for a simple explanatory demo.

### Register your new app in `apps.h`

1. `#include` the file containing your app object.
2. Add the app object to the apps[] array

### Methods in your app

#### char* getMenuName()

Return the name of the app as you want to it appear in the menu.

#### const unsigned short* getIcon()

Return an app icon to be displayed on the menu. It should be 64x64 and in the format for pushImage.<sup id="a1">[1](#f1)</sup>
This will also be the namespace for the config.

#### void onSetup(TFT_eSPI tft)

This is like the Arduino setup function, but run every time your app loads up.

Initialise your variables here.

You are also given the tft object<sup id="a2">[2](#f2)</sup> so you display a loading/splash/intial screen.

#### void render(TFT_eSPI tft)

This is like the Arduino loop function, repeatedly called while your app is open.

You are given the tft object<sup id="a3">[2](#f2)</sup> so you can render your application.

#### void onClose()

This is run when your application is being closed (because the user exited).

Clean up to prevent memory leaks if needed here.

#### void onButton*Click()
##### void onButton1Click()
##### void onButton1LongClick()
##### void onButton2Click()

Triggered when a button is clicked, but only when your app is open.

You may add your own listeners, but you must be careful not to affect anything while your app is not open.

#### bool getFullscreen()

Return true if the status bar should be rendered with your app.

#### bool getSleepDisabled()

Return true if the device should not sleep while your app is open. You should consider the power consumption implications if you do so.

#### JsonObject getSettings()

This is for use by your application to get the config object for your app. This will be from the menu name's key in the global config object.

For example, if your app's name was "Foo", then the config should look like this:
```json
{
  ...
  "Foo": {
    "bar": "asdf",
    "bah": [1, 3, 5]
  },
  ...
}
```
and when your app calls `getSettings()` you will receive a [JsonObject](https://arduinojson.org/v6/api/jsonobject/) like
```json
{
  "bar": "asdf",
  "bah": [1, 3, 5]
}
```

# Footnotes

<b id="f1">1</b> You can use [Henning Karlsen's Image Converter](http://www.rinkydinkelectronics.com/t_imageconverter565.php) to generate these. [↩](#a2)

<b id="f2">2</b> For general graphics, [Bodmer's TFT_eSPI library](https://github.com/Bodmer/TFT_eSPI) was used. [↩](#a1)

