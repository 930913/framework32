// Include apps here
#include "Menu.h"
#include "About.h"
#include "Breakout.h"
#include "Hive.h"
#include "WifiScan.h"
#include "BbcNews.h"
#include "DemoApp.h"
#include "BitcoinTracker.h"

// Register apps here
App* apps[] = {
  &menu,
  &bitcointracker,
  &breakout,
  &hive,
  &bbcnews,
  &wifiscan,
  &demoapp,
  &about
};
