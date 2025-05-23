
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

#include <Adafruit_INA228.h>
Adafruit_INA228 ina228 = Adafruit_INA228();
float shuntvoltage = 0;
float busvoltage = 0;
float max_busvoltage = 0;
float current_mA = 0;
float max_current_mA = 0;
float loadvoltage = 0;
float power_mW = 0;
float max_power_mW = 0;
float temp = 0;
float capacity = 0;
bool ina_inicialized = false;
volatile bool updateLabels = false;
void meassure();

#include "ui/ui.h"

extern lv_obj_t *ui_Chart1;
constexpr int32_t HOR_RES = 320;
constexpr int32_t VER_RES = 240;

#define SLEEP_TIME 120000
static int16_t stored_brightness = 127;
static bool is_dimmed = false;
static TimerHandle_t displayTimer = NULL;

lv_display_t *display;
lv_indev_t *indev;

void beep();
void beep2();
void click();
void updateBattery();
void setupDisplayTimer();

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

    // Reset LVGL's inactivity counter on touch
    lv_disp_trig_activity(NULL);
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

  delay(2500);

  M5.begin();
  M5.Power.begin();
  M5.Display.setBrightness(stored_brightness);
  delay(100);

  lv_init();
  lv_tick_set_cb(my_tick_function);

  display = lv_display_create(HOR_RES, VER_RES);
  lv_display_set_flush_cb(display, my_display_flush);

  static lv_color_t buf1[HOR_RES * 15];
  lv_display_set_buffers(display, buf1, nullptr, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

  indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);

  lv_indev_set_read_cb(indev, my_touchpad_read);

  // Wire.end();
  if (!ina228.begin())
  {
    ESP_LOGE(TAG, "Failed to find INA228 chip");
  }
  else
  {
    ina_inicialized = true;
    ESP_LOGI(TAG, "INA228 chip found");
  }

  if (ina_inicialized)
  {
    ina228.setShunt(0.015, 10.0);

    ina228.setAveragingCount(INA228_COUNT_16);
    uint16_t counts[] = {1, 4, 16, 64, 128, 256, 512, 1024};
    Serial.print("Averaging counts: ");
    Serial.println(counts[ina228.getAveragingCount()]);

    // set the time over which to measure the current and bus voltage
    ina228.setVoltageConversionTime(INA228_TIME_540_us);
    Serial.print("Voltage conversion time: ");
    switch (ina228.getVoltageConversionTime())
    {
    case INA228_TIME_50_us:
      Serial.print("50");
      break;
    case INA228_TIME_84_us:
      Serial.print("84");
      break;
    case INA228_TIME_150_us:
      Serial.print("150");
      break;
    case INA228_TIME_280_us:
      Serial.print("280");
      break;
    case INA228_TIME_540_us:
      Serial.print("540");
      break;
    case INA228_TIME_1052_us:
      Serial.print("1052");
      break;
    case INA228_TIME_2074_us:
      Serial.print("2074");
      break;
    case INA228_TIME_4120_us:
      Serial.print("4120");
      break;
    }
    Serial.println(" uS");

    ina228.setCurrentConversionTime(INA228_TIME_280_us);
    Serial.print("Current conversion time: ");
    switch (ina228.getCurrentConversionTime())
    {
    case INA228_TIME_50_us:
      Serial.print("50");
      break;
    case INA228_TIME_84_us:
      Serial.print("84");
      break;
    case INA228_TIME_150_us:
      Serial.print("150");
      break;
    case INA228_TIME_280_us:
      Serial.print("280");
      break;
    case INA228_TIME_540_us:
      Serial.print("540");
      break;
    case INA228_TIME_1052_us:
      Serial.print("1052");
      break;
    case INA228_TIME_2074_us:
      Serial.print("2074");
      break;
    case INA228_TIME_4120_us:
      Serial.print("4120");
      break;
    }
    Serial.println(" uS");
  }

  ui_init(); // Initialize UI generated by Square Line
  setupDisplayTimer();

  M5.Speaker.begin();
  M5.Speaker.setVolume(42);
  /// Using a single wave of data, you can change the tone.
  M5.Speaker.tone(261.626, 1000, 1, false, wavdata, sizeof(wavdata));
  delay(200);
  M5.Speaker.tone(329.628, 1000, 2, false, wavdata, sizeof(wavdata));
  delay(200);
  M5.Speaker.tone(391.995, 800, 3, false, wavdata, sizeof(wavdata));
  delay(2000);
}

void loop()
{
  M5.update();

  lv_task_handler();

  static uint32_t last_activity = lv_disp_get_inactive_time(NULL);

  if (lv_disp_get_inactive_time(NULL) > SLEEP_TIME)
  {
    if (!is_dimmed)
    {
      stored_brightness = M5.Display.getBrightness();
      M5.Display.setBrightness(0);
      is_dimmed = true;
    }
  }
  else
  {
    if (is_dimmed)
    {
      M5.Display.setBrightness(stored_brightness);
      is_dimmed = false;
    }
  }

  if (updateLabels)
  {
    updateLabels = false;
    char voltage_str[8];
    char power_str[8];
    char current_str[8];
    char max_current_str[8];
    char max_power_str[8];
    char max_busvoltage_str[8];
    char capacity_str[8];
    char temp_str[8];

    snprintf(voltage_str, sizeof(voltage_str), "%.2f", busvoltage);
    snprintf(power_str, sizeof(power_str), "%.2f", power_mW / 1000.0);
    snprintf(current_str, sizeof(current_str), "%.0f", current_mA);
    snprintf(max_current_str, sizeof(max_current_str), "%.0f", max_current_mA);
    snprintf(max_power_str, sizeof(max_power_str), "%.1f", max_power_mW / 1000.0);
    snprintf(max_busvoltage_str, sizeof(max_busvoltage_str), "%.1f", max_busvoltage);
    snprintf(capacity_str, sizeof(capacity_str), "%.1f", capacity);
    snprintf(temp_str, sizeof(temp_str), "%.1f", temp);

    lv_label_set_text(ui_Label_Voltage, voltage_str);
    lv_label_set_text(ui_Label_Power, power_str);
    lv_label_set_text(ui_Label_Current, current_str);
    lv_label_set_text(ui_Label_Capacity, capacity_str);
    lv_label_set_text(ui_Label_Max_current, max_current_str);
    lv_label_set_text(ui_Label_Max_Power, max_power_str);
    lv_label_set_text(ui_Label_Max_Voltage, max_busvoltage_str);
    lv_label_set_text(ui_Label_Temperature, temp_str);
  }

  if (is_dimmed)
  {
    vTaskDelay(pdMS_TO_TICKS(50)); // Pomalejší aktualizace při spánku
  }
  else
  {
    vTaskDelay(pdMS_TO_TICKS(5)); // Normální rychlost
  }
}

void click()
{
  M5.Speaker.tone(5000, 50, 1, true, wavdata, sizeof(wavdata));
  // M5.Speaker.tone(400, 500, 1, true, wavdata, sizeof(wavdata));
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

void CapacityClicked(lv_event_t *e)
{
  if (is_dimmed)
  {
    return;
  }

  click();
  capacity = 0;
}

void CurrentClicked(lv_event_t *e)
{
  if (is_dimmed)
  {
    return;
  }

  click();
  max_current_mA = 0;
}

void PowerClicked(lv_event_t *e)
{
  if (is_dimmed)
  {
    return;
  }

  click();
  max_power_mW = 0;
}
void VoltageClicked(lv_event_t *e)
{
  if (is_dimmed)
  {
    return;
  }

  click();
  max_busvoltage = 0;
}

void displayUpdateValuesTimmer(TimerHandle_t xTimer)
{
  meassure();
  if (is_dimmed)
  {
    return; // Don't update if the display is dimmed
  }
  updateBattery();
}

void setupDisplayTimer()
{
  displayTimer = xTimerCreate(
      "DisplayTimer",           // Timer name
      pdMS_TO_TICKS(1000),      // Timer period in ticks (1000ms)
      pdTRUE,                   // Auto reload
      (void *)0,                // Timer ID
      displayUpdateValuesTimmer // Callback function
  );

  if (displayTimer != NULL)
  {
    xTimerStart(displayTimer, 0);
  }
}

void updateBattery()
{
  int32_t batPercentage = M5.Power.getBatteryLevel();
  lv_bar_set_value(ui_Battery, batPercentage, LV_ANIM_ON);

  int32_t current = M5.Power.getBatteryCurrent();
  if (current > 0)
  {
    lv_obj_clear_flag(ui_ScreenMain_Charging, LV_OBJ_FLAG_HIDDEN);
  }
  else
  {
    lv_obj_add_flag(ui_ScreenMain_Charging, LV_OBJ_FLAG_HIDDEN);
  }

  // Pro debug můžeme vypsat hodnoty do sériové konzole
  // ESP_LOGI(TAG, "Battery: %d%%, current: %d", batPercentage, current);
}

void meassure()
{
  if (!ina_inicialized)
    return;

  shuntvoltage = ina228.readShuntVoltage();
  busvoltage = ina228.readBusVoltage() / 1000000.0;
  current_mA = ina228.readCurrent();
  power_mW = ina228.readPower();
  temp = ina228.readDieTemp();

  if (busvoltage > max_busvoltage)
  {
    max_busvoltage = busvoltage;
  }
  if (current_mA > max_current_mA)
  {
    max_current_mA = current_mA;
  }
  if (power_mW > max_power_mW)
  {
    max_power_mW = power_mW;
  }

  // Calculate capacity in mAh based on current measured every 1000ms
  capacity += current_mA / 3600.0; // Convert mA measured over 1 second to mAh

  // Serial.print("Temperature: ");
  // Serial.print(temp);
  // Serial.println(" *C");
  // Serial.print("Energy: ");
  // Serial.print(ina228.readEnergy());
  // Serial.println(" J");
  // Serial.print("Bus Voltage:   ");
  // Serial.print(busvoltage);
  // Serial.println(" V");
  // Serial.print("Shunt Voltage: ");
  // Serial.print(shuntvoltage);
  // Serial.println(" mV");
  // Serial.print("Current:       ");
  // Serial.print(current_mA);
  // Serial.println(" mA");
  // Serial.print("Power:         ");
  // Serial.print(power_mW);
  // Serial.println(" mW\n");

  updateLabels = true;
}