static const char *TAG = "main";

// #include <SD.h>
// #include <FS.h>
// #include <SPI.h>
// #include <vector>
#include <Arduino.h>
#include <M5Unified.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include <string.h>
#include <stdio.h>
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define LV_CONF_INCLUDE_SIMPLE
#include <lvgl.h>
#include <esp_timer.h>
// #include "AccelSensor.hpp"

#include "ui/ui.h"

#include <NimBLEDevice.h>

extern lv_obj_t *ui_Chart1;
constexpr int32_t HOR_RES = 320;
constexpr int32_t VER_RES = 240;

lv_display_t *display;
lv_indev_t *indev;

void beep();
void beep2();

void my_display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  lv_draw_sw_rgb565_swap(px_map, w * h);
  M5.Display.pushImageDMA<uint16_t>(area->x1, area->y1, w, h, (uint16_t *)px_map);
  lv_disp_flush_ready(disp);
}

uint32_t my_tick_function()
{
  return (esp_timer_get_time() / 1000LL);
}

void my_touchpad_read(lv_indev_t *drv, lv_indev_data_t *data)
{
  M5.update();
  auto count = M5.Touch.getCount();

  if (count == 0)
  {
    data->state = LV_INDEV_STATE_RELEASED;
  }
  else
  {
    auto touch = M5.Touch.getDetail(0);
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = touch.x;
    data->point.y = touch.y;
  }
}

const uint8_t wavdata[64] = {132, 138, 143, 154, 151, 139, 138, 140, 144, 147, 147, 147, 151, 159, 184, 194, 203, 222, 228, 227, 210, 202, 197, 181, 172, 169, 177, 178, 172, 151, 141, 131, 107, 96, 87, 77, 73, 66, 42, 28, 17, 10, 15, 25, 55, 68, 76, 82, 80, 74, 61, 66, 79, 107, 109, 103, 81, 73, 86, 94, 99, 112, 121, 129};

// continue setup code
void setup()
{
  esp_log_level_set("*", ESP_LOG_INFO);

  Serial.begin(115200);
  // Wire.begin();
  // Wire.setClock(400000);

  M5.begin();
  M5.Power.begin();
  M5.Display.setBrightness(90);
  delay(100);

  M5.Speaker.begin();
  M5.Speaker.setVolume(42);
  /// Using a single wave of data, you can change the tone.
  M5.Speaker.tone(261.626, 1000, 1, false, wavdata, sizeof(wavdata));
  delay(200);
  M5.Speaker.tone(329.628, 1000, 2, false, wavdata, sizeof(wavdata));
  delay(200);
  M5.Speaker.tone(391.995, 800, 3, false, wavdata, sizeof(wavdata));
  delay(200);

  lv_init();
  lv_tick_set_cb(my_tick_function);

  display = lv_display_create(HOR_RES, VER_RES);
  lv_display_set_flush_cb(display, my_display_flush);

  static lv_color_t buf1[HOR_RES * 15];
  lv_display_set_buffers(display, buf1, nullptr, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

  indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);

  lv_indev_set_read_cb(indev, my_touchpad_read);

  ui_init(); // Initialize UI generated by Square Line

  NimBLEDevice::init("NimBLE");
  NimBLEDevice::setPower(3); /** +3db */

  NimBLEDevice::setSecurityAuth(true, true, true); /** bonding, MITM, BLE secure connections */
  NimBLEDevice::setSecurityPasskey(123456);
  NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY); /** Display only passkey */
  NimBLEServer *pServer = NimBLEDevice::createServer();
  NimBLEService *pService = pServer->createService("ABCD");
  NimBLECharacteristic *pNonSecureCharacteristic = pService->createCharacteristic("1234", NIMBLE_PROPERTY::READ);
  NimBLECharacteristic *pSecureCharacteristic =
      pService->createCharacteristic("1235",
                                     NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN);

  pService->start();
  pNonSecureCharacteristic->setValue("Hello Non Secure BLE");
  pSecureCharacteristic->setValue("Hello Secure BLE");

  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID("ABCD");
  pAdvertising->start();
}

void loop()
{
  M5.update();
  lv_task_handler();
  vTaskDelay(1);
}

void beep()
{
  M5.Speaker.tone(500, 1000, 1, true, wavdata, sizeof(wavdata));
  // M5.Speaker.tone(400, 500, 1, true, wavdata, sizeof(wavdata));
}

void beep2()
{
  M5.Speaker.tone(1000, 300, 1, true, wavdata, sizeof(wavdata));
  M5.Speaker.tone(2000, 300, 1, true, wavdata, sizeof(wavdata));
}
