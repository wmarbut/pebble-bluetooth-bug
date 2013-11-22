#include <pebble.h>

static Window *window;
static TextLayer *bt_callback_layer;
static TextLayer *bt_peek_layer;


void bluetooth_callback(bool connected) {
	static char str[] = "CB: false";
	if (connected) {
		snprintf(str, sizeof(str), "CB: true");
	}
	text_layer_set_text(bt_callback_layer, str);
}

void update_peek(bool connected) {
	static char str[] = "Peek: false";
	if (connected) {
		snprintf(str, sizeof(str), "Peek: true");
	}
	text_layer_set_text(bt_peek_layer, str);
}

void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
	bool is_connected = bluetooth_connection_service_peek();
	update_peek(is_connected);
}


static void window_load(Window *window) {
  bool is_connected = bluetooth_connection_service_peek();
  Layer *window_layer = window_get_root_layer(window);

  bt_callback_layer = text_layer_create(GRect(5,5, 144-5, 25));
  bluetooth_callback(is_connected);
  layer_add_child(window_layer, text_layer_get_layer(bt_callback_layer));

  bt_peek_layer = text_layer_create(GRect(5,35, 144-5, 25));
  update_peek(is_connected);
  layer_add_child(window_layer, text_layer_get_layer(bt_peek_layer));

  bluetooth_connection_service_subscribe( bluetooth_callback );
}

static void window_unload(Window *window) {
  text_layer_destroy(bt_callback_layer);
  text_layer_destroy(bt_peek_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit(void) {
  window_destroy(window);
  bluetooth_connection_service_unsubscribe();
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
