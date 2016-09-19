#include <pebble.h>
#include <time.h>
#include "main.h"

#define FONT_HEIGHT 40
#define HOUR_LENGTH 3600
#define DAY_LENGTH 86400


static Window *s_main_window;
static TextLayer *s_time_layer;
static int last_time = 0;

/** 
 * Computes the current .beat time
 */
static int beat() {
  time_t now = time(NULL) + HOUR_LENGTH;
  time_t day_now = now % DAY_LENGTH;
  int b = (day_now * 10000) / DAY_LENGTH;
  return b;
}

/**
 * Update time display
 */
static void update_time() {
  int now =  beat();
  
  if (now == last_time) {
    return;
  }
  
  static char disp[8];
  int n = snprintf(disp, 7, "@%d", now);
  disp[n] = disp[n - 1];
  disp[n - 1] = '.';
  disp[n + 1] = '\0';
  
  text_layer_set_text(s_time_layer, disp);
  
  last_time = now;
}

/**
 * Handles click ticks
 */
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_time_layer = text_layer_create(
      GRect(0, (bounds.size.h - FONT_HEIGHT) / 2, bounds.size.w, 50));
  
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "@0");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
}

static void init() {
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(s_main_window, true);
  
  update_time();
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}