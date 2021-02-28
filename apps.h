// Include apps here
#include "Menu.h"
#include "About.h"
#include "BbcNews.h"
#include "BitcoinTracker.h"
#include "Breakout.h"
#include "DemoApp.h"
#include "Hive.h"
#include "LichessTv.h"
#include "Octoprint.h"
//#include "Otp.h"
#include "WifiScan.h"

// Register apps here
App* apps[] = {
  &menu,
  &lichesstv,
  &octoprint,
  &bitcointracker,
  &breakout,
  &hive,
  &bbcnews,
  //&otp,
  &wifiscan,
  &demoapp,
  &about
};
