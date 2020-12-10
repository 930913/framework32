void fillScreen(TFT_eSPI tft, uint32_t colour) {
    tft.fillRect(0, 10, TFT_WIDTH, TFT_HEIGHT - 10, colour);
}

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{
  esp_sleep_enable_timer_wakeup(ms * 1000);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
  esp_light_sleep_start();
}
