#include <pebble.h>
#include <time.h>

static Window *s_main_window;
static TextLayer *s_hmstime_layer;
static TextLayer *s_date_layer;
static TextLayer *s_mjd_layer;
static TextLayer *s_time_layer;

void ftoa(char* str, double val, int precision) {
  //  start with positive/negative
  if (val < 0) {
    *(str++) = '-';
    val = -val;
  }
  //  integer value
  snprintf(str, 12, "%d", (int) val);
  str += strlen(str);
  val -= (int) val;
  //  decimals
  if ((precision > 0) && (val >= .00001)) {
    //  add period
    *(str++) = '.';
    //  loop through precision
    for (int i = 0;  i < precision;  i++)
      if (val > 0) {
        val *= 10;
        *(str++) = '0' + (int) (val + ((i == precision - 1) ? .5 : 0));
        val -= (int) val;
      } else
        break;
  }
  //  terminate
  *str = '\0';
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

static void update_time() {
  // Get a tm structure
  time_t t = time(NULL); 
  struct tm *tick_time = gmtime(&t);
  
  long mjd, stod, year, month, day, hour, minute, second;
  double fmjd;
  
  
  year   = tick_time->tm_year + 1900;
  month  = tick_time->tm_mon + 1;
  day    = tick_time->tm_mday;
  hour   = tick_time->tm_hour;
  minute = tick_time->tm_min;
  second = tick_time->tm_sec;
  
  mjd = date_to_mjd(year, month, day);
  stod = (((hour * 60) + minute) * 60) + second;
  fmjd = mjd + (stod / 86400.0);
  
  // Create a long-lived buffer
  static char buffer[] = "00:00:00";
  static char datebuffer[] = "00/00/00";
  static char mjdbuffer[] = "00000.0000000";
  static char timebuffer[] = "Tue 11 Nov";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00:00"), "%H:%M:%S", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00:00"), "%I:%M:%S", tick_time);
  }
  
  strftime(datebuffer, sizeof("00/00/00"), "%d/%m/%y", tick_time);
  
  //snprintf(mjdbuffer, sizeof("00000.0000000"), "%.7f", fmjd);
  ftoa(mjdbuffer,fmjd, 7);
  
  strftime(timebuffer, sizeof("Sun 01 Aug"), "%a %b %d", tick_time);
  
  // Display this time on the TextLayer
  text_layer_set_text(s_hmstime_layer, buffer);
  text_layer_set_text(s_date_layer, datebuffer);
  text_layer_set_text(s_mjd_layer, mjdbuffer);
  text_layer_set_text(s_time_layer, timebuffer);
  
}

static void main_window_load(Window *window) {
  // Create time TextLayer
  s_hmstime_layer = text_layer_create(GRect(20, 65, 100, 40));
  text_layer_set_background_color(s_hmstime_layer, GColorBlack);
  text_layer_set_text_color(s_hmstime_layer, GColorWhite);
  text_layer_set_text(s_hmstime_layer, "00:00");
  
  s_date_layer = text_layer_create(GRect(44, 90, 100, 40));
  text_layer_set_background_color(s_date_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "00/00/00");
  
  s_mjd_layer = text_layer_create(GRect(0, 135, 144, 35));
  text_layer_set_background_color(s_mjd_layer, GColorBlack);
  text_layer_set_text_color(s_mjd_layer, GColorWhite);
  text_layer_set_text(s_mjd_layer, "00000.0000000");
  
  s_time_layer = text_layer_create(GRect(0, 0, 144, 35));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "Sun 01 aug");
  

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_hmstime_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(s_hmstime_layer, GTextAlignmentCenter);

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_mjd_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_mjd_layer, GTextAlignmentCenter);
  
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hmstime_layer));
  //layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_mjd_layer));
   layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
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

