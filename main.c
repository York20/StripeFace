#include <pebble.h>
Window *window;
TextLayer *text_layer, *battery_layer, *date_layer, *battery_background;

//Creates arrays capible of storing data.
static char time_format[] = "00:00:00";
static char date_format[11];
static char battery_format[] = "100%";

//Handles event changes related to time and allows the time pointers to be used for exact time data straight from the unix hardware clock.
void tick_handler(struct tm *t, TimeUnits units_changed){
  
  strftime(date_format, sizeof(date_format), "%a %d", t);
  text_layer_set_text(date_layer, date_format);
  
  //Checks to see if the what style the user has set their pebble watch to and writes accordingly.
  if (clock_is_24h_style()){
    strftime(time_format, sizeof(time_format), "%I:%M:%S", t);
  }
  else{
    strftime(time_format, sizeof(time_format), "%H:%M:%S", t);
  }
  
  //Writes text to layer.
  text_layer_set_text(text_layer, time_format);
  text_layer_set_text(date_layer, date_format);
  
}

void battery_handler(BatteryChargeState state){
  int percent = state.charge_percent;
  snprintf(battery_format, sizeof(battery_format), "%d%%", percent);
  text_layer_set_text(battery_layer, battery_format);
  
}

void init(){
  
  //Initialisation of layers and objects.
  window = window_create();
  
  //Subscribes to needed services for data to be pulled.
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
  
  //Gets boundaries of window.
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  //Creates layers.
  text_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(0,0),PBL_IF_ROUND_ELSE(70,66),bounds.size.w,PBL_IF_ROUND_ELSE(65,61)));
  battery_background = text_layer_create(GRect(PBL_IF_ROUND_ELSE(0,0),PBL_IF_ROUND_ELSE(0,0),bounds.size.w,PBL_IF_ROUND_ELSE(70,67)));
  date_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(0,0),PBL_IF_ROUND_ELSE(117,112),bounds.size.w, PBL_IF_ROUND_ELSE(65,61)));
  battery_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(6,4),PBL_IF_ROUND_ELSE(15,10),bounds.size.w, PBL_IF_ROUND_ELSE(31,28)));
  
  //Centres Text.
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(battery_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  
  //Adds to window.
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  layer_add_child(window_layer, text_layer_get_layer(date_layer));
  layer_add_child(window_layer, text_layer_get_layer(battery_background));
  layer_add_child(window_layer, text_layer_get_layer(battery_layer));
  
  //Sets Font.
  text_layer_set_font(text_layer, PBL_IF_ROUND_ELSE(fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK), fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD)));
  text_layer_set_font(battery_layer, PBL_IF_ROUND_ELSE(fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK), fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD)));
  text_layer_set_font(date_layer, PBL_IF_ROUND_ELSE(fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK), fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD)));
  
  //Sets Background
  text_layer_set_background_color(text_layer, PBL_IF_BW_ELSE(GColorWhite, GColorYellow));
  text_layer_set_background_color(battery_background, PBL_IF_BW_ELSE(GColorBlack, GColorRed));
  text_layer_set_background_color(battery_layer, GColorClear);
  text_layer_set_background_color(date_layer, PBL_IF_BW_ELSE(GColorBlack, GColorBlue));
  
  //Sets Text Color For Aplite.
  text_layer_set_text_color(battery_layer, PBL_IF_BW_ELSE(GColorWhite, GColorBlack));
  text_layer_set_text_color(date_layer, PBL_IF_BW_ELSE(GColorWhite, GColorBlack));
  
  //Calls update functions.
  BatteryChargeState state = battery_state_service_peek();
  battery_handler(state);
  
  //Push window to the top of the stack allowing it to be seen.
  window_stack_push(window, true);
}

void deinit(){
  //Destroys created objects allowing memory and resources to be freed.
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  text_layer_destroy(text_layer);
  text_layer_destroy(battery_background);
  text_layer_destroy(date_layer);
  text_layer_destroy(battery_layer);
  window_destroy(window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}
