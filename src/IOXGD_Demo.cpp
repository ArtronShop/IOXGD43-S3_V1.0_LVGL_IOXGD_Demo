#include "lvgl.h"

extern void home_page();
extern void graph_page();
extern void gpio_page();
extern void calculator_page();
extern void system_page();
extern void wifiscan_page();

extern lv_obj_t* homeScreen;
extern lv_obj_t* gpioScreen;
extern lv_obj_t* graphScreen;
extern lv_obj_t* calculatorScreen;
extern lv_obj_t* wifiscanScreen;
extern lv_obj_t* systemScreen;

extern lv_obj_t* chart1;
extern lv_obj_t* slider1;

void IOXGD_Demo() {
  home_page();
  graph_page();
  gpio_page();
  calculator_page();
  system_page();
  wifiscan_page();

  lv_scr_load(homeScreen);
}
