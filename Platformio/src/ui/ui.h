// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.1
// LVGL version: 9.1.0
// Project name: battery_monitor

#ifndef _BATTERY_MONITOR_UI_H
#define _BATTERY_MONITOR_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined __has_include
  #if __has_include("lvgl.h")
    #include "lvgl.h"
  #elif __has_include("lvgl/lvgl.h")
    #include "lvgl/lvgl.h"
  #else
    #include "lvgl.h"
  #endif
#else
  #include "lvgl.h"
#endif

#include "ui_helpers.h"
#include "ui_events.h"

// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
extern lv_obj_t *ui_Screen1;
extern lv_obj_t *ui_Label1;
extern lv_obj_t *ui_StatusBar;
extern lv_obj_t *ui_Clock;
extern lv_obj_t *ui_ClockLabel;
extern lv_obj_t *ui_BatOuter;
extern lv_obj_t *ui_ScreenMain_Bat;
extern lv_obj_t *ui_Battery;
extern lv_obj_t *ui_ScreenMain_ContainerBatt;
extern lv_obj_t *ui_ScreenMain_Charging;
extern lv_obj_t *ui_ScreenMain_Label3;
extern lv_obj_t *ui_ScreenMain_Label4;
extern lv_obj_t *ui_Container1;
extern lv_obj_t *ui_Container_Voltage;
extern lv_obj_t *ui_Label4;
extern lv_obj_t *ui_Label_Voltage;
extern lv_obj_t *ui_Label5;
void ui_event_Container_Capacity( lv_event_t * e);
extern lv_obj_t *ui_Container_Capacity;
extern lv_obj_t *ui_Label6;
extern lv_obj_t *ui_Label_Capacity;
extern lv_obj_t *ui_Label7;
extern lv_obj_t *ui_Container_Power;
extern lv_obj_t *ui_Label3;
extern lv_obj_t *ui_Label_Power;
extern lv_obj_t *ui_Label2;
// CUSTOM VARIABLES

// EVENTS
extern lv_obj_t *ui____initial_actions0;

// FONTS
LV_FONT_DECLARE( ui_font_H1);
LV_FONT_DECLARE( ui_font_H3);

// UI INIT
void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
