#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_hmstime_layer;
static TextLayer *s_date_layer;
static TextLayer *s_mjd_layer;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00:00";
  static char datebuffer[] = "00/00/00";
  static char mjdbuffer[] = "00000.0000000";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00:00"), "%H:%M:%S", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00:00"), "%I:%M:%S", tick_time);
  }
  
  strftime(datebuffer, sizeof("00/00/00"), "%d/%m/%y", tick_time);
  strftime(mjdbuffer, sizeof("Sun 01 Aug"), "%a %b %d", tick_time);
  
  
  // Display this time on the TextLayer
  text_layer_set_text(s_hmstime_layer, buffer);
  text_layer_set_text(s_date_layer, datebuffer);
   text_layer_set_text(s_mjd_layer, mjdbuffer);
}

static void main_window_load(Window *window) {
  // Create time TextLayer
  s_hmstime_layer = text_layer_create(GRect(0, 20, 100, 40));
  text_layer_set_background_color(s_hmstime_layer, GColorBlack);
  text_layer_set_text_color(s_hmstime_layer, GColorWhite);
  text_layer_set_text(s_hmstime_layer, "00:00");
  
  s_date_layer = text_layer_create(GRect(44, 65, 100, 40));
  text_layer_set_background_color(s_date_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "00/00/00");
  
  s_mjd_layer = text_layer_create(GRect(0, 110, 144, 25));
  text_layer_set_background_color(s_mjd_layer, GColorBlack);
  text_layer_set_text_color(s_mjd_layer, GColorWhite);
  text_layer_set_text(s_mjd_layer, "00000.0000000");
  

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_hmstime_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(s_hmstime_layer, GTextAlignmentCenter);

    // Improve the layout to be more like a watchface
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_mjd_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_mjd_layer, GTextAlignmentCenter);

  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hmstime_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
   layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_mjd_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_hmstime_layer);
   text_layer_destroy(s_date_layer);
   text_layer_destroy(s_mjd_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

long date_to_mjd (long year, long month, long day)
{
    return
        367 * year
        - 7 * (year + (month + 9) / 12) / 4
        - 3 * ((year + (month - 9) / 7) / 100 + 1) / 4
        + 275 * month / 9
        + day
        + 1721028
        - 2400000;
}