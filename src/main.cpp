#include "Arduino.h"
#include "Wire.h"
#include "PinCofigs.h"
#include "LCD.h"
#include <lvgl.h>
#include "FT5216.h"

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 800;
static const uint16_t screenHeight = 480;

static const uint32_t buffer_size = screenWidth * 400 * sizeof(lv_color_t);
static lv_disp_buf_t draw_buf;
lv_color_t *buf;
/*
typedef struct  {
  lv_disp_drv_t *disp; 
  lv_area_t *area;
  lv_color_t *color_p;
} DisplayFlushInfo;

QueueHandle_t displayFlushQueue = NULL;
*/
/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )  {
    LCD_drawBitmap(area->x1, area->y1, area->x2, area->y2, &color_p->full);

    lv_disp_flush_ready(disp);
    /*DisplayFlushInfo flushInfo = {
      .disp = disp,
      .area = (lv_area_t*)area,
      .color_p = color_p
    };
    xQueueSend(displayFlushQueue, (void*)&flushInfo, 0); */
}

/*Read the touchpad*/
bool my_touchpad_read( lv_indev_drv_t *indev_driver, lv_indev_data_t * data) {
  uint8_t touchPoint = touch.read((uint16_t*)(&data->point.x), (uint16_t*)(&data->point.y));
  if (touchPoint > 0) {
    Serial.printf("X: %d\tY: %d\n", data->point.x, data->point.y);
  }
  data->state = touchPoint > 0 ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

  return false;
}
/*
TaskHandle_t bufferPutTaskHandle = NULL;

void bufferPutTask(void *args) {
  while(1) {
    DisplayFlushInfo flushInfo;
    if(xQueueReceive(displayFlushQueue, &flushInfo, portMAX_DELAY) != pdPASS) {
      continue;
    }

    LCD_drawBitmap(flushInfo.area->x1, flushInfo.area->y1, flushInfo.area->x2, flushInfo.area->y2, &flushInfo.color_p->full);
    lv_disp_flush_ready(flushInfo.disp);
  }
  vTaskDelete(NULL);
}*/
EventGroupHandle_t systemEventGroup = NULL;

extern void IOXGD_Demo();

void beep() {
  xEventGroupSetBits(systemEventGroup, 0x01);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, 400E3);

  LCD_init();
  touch.init();

  lv_init();
  buf = (lv_color_t*) heap_caps_malloc(buffer_size, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
  if (!buf) {
    Serial.println("malloc fail!");
    while(1) delay(10);
  }
  lv_disp_buf_init(&draw_buf, buf, NULL, buffer_size);

  /*displayFlushQueue = xQueueCreate(5, sizeof(DisplayFlushInfo));
  if(!displayFlushQueue) {
    Serial.println("Display Flush Queue was not created and must not be used.");
    while(1) delay(10);
  }
  xTaskCreatePinnedToCore(bufferPutTask, "bufferPutTask", 1024, NULL, 20, &bufferPutTaskHandle, 1 - ARDUINO_RUNNING_CORE);*/

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);

  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
/*
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello !, LVGL");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
*/
  Serial.println("Setup done");

  IOXGD_Demo();

  systemEventGroup = xEventGroupCreate();

  if (systemEventGroup != NULL) {
    xTaskCreate([](void*) {
      const int freq = 2000;
      const int ledChannel = 0;
      const int resolution = 8;

      // configure LED PWM functionalitites
      ledcSetup(ledChannel, freq, resolution);
      
      // attach the channel to the GPIO to be controlled
      ledcAttachPin(BUZZER_PIN, ledChannel);

      while (1) {
        EventBits_t uxBits = xEventGroupWaitBits(systemEventGroup, 0x01, pdTRUE, pdFALSE, portMAX_DELAY);
        if((uxBits & 0x01) != 0) {
          ledcWrite(ledChannel, 127);
          vTaskDelay(50 / portTICK_PERIOD_MS);
          ledcWrite(ledChannel, 0);
        }
      }
    }, "soundTask", 4 * 1024, NULL, 5, NULL);
  } else {
    Serial.println("The event group was not created because there was insufficient, FreeRTOS heap available.");
  }
}

void loop() {
  lv_task_handler(); /* let the GUI do its work */
  delay(5);
}
