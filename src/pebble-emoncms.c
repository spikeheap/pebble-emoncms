#include <pebble.h>

enum {
    AKEY_NUMBER,
    AKEY_TEXT,
};

enum MessageKey {
  EMON_TEMPERATURE_KEY = 0x0,  // TUPLE_CSTRING
  EMON_HEATING_STATE_KEY = 0x1,
  EMON_FEED_INDEX_KEY = 0x2,
  EMON_FEED_COUNT_KEY = 0x3
};

static AppSync sync;
static uint8_t sync_buffer[64];

static Window *window;
static TextLayer *text_layer;

static char heatingOn = '0';
static int feedCount = 1;
static int feedIndex = 0;

// Decrement the selected feed index
static void down_click_handler(ClickRecognizerRef recognizer, void* context) {
  if(feedIndex <= 0){
    feedIndex = feedCount;
  }else{
    feedIndex = feedIndex-1;
  }
  Tuplet new_tuples[] = { 
    TupletInteger(EMON_FEED_INDEX_KEY, feedIndex) 
  };
  app_sync_set(&sync, new_tuples, 1);
}

// Increment the selected feed index
static void up_click_handler(ClickRecognizerRef recognizer, void* context) {
  if(feedIndex >= feedCount){
    feedIndex = 0;
  }else{
    feedIndex = feedIndex+1;
  }
  
  Tuplet new_tuples[] = { 
    TupletInteger(EMON_FEED_INDEX_KEY, feedIndex) 
  };
  app_sync_set(&sync, new_tuples, 1);
}


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //DictionaryIterator *iter;
  //app_message_outbox_begin(&iter);

  if(heatingOn){
    //Tuplet value = TupletInteger(1, 1);
    //dict_write_tuplet(iter, &value);
    Tuplet new_tuples[] = {
        TupletCString(EMON_HEATING_STATE_KEY, "0"),
    };
    heatingOn = false;
    app_sync_set(&sync, new_tuples, 1);
  }else{
    //Tuplet value = TupletInteger(1, 0);
    //dict_write_tuplet(iter, &value);
    Tuplet new_tuples[] = {
        TupletCString(EMON_HEATING_STATE_KEY, "1"),
    };
    heatingOn = true;
    app_sync_set(&sync, new_tuples, 1);
  }
  
  //text_layer_set_text(text_layer, heatingOn);
}

 // Messaging
static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  switch (key) {

    case EMON_TEMPERATURE_KEY:
        // App Sync keeps new_tuple in sync_buffer, so we may use it directly
        text_layer_set_text(text_layer, new_tuple->value->cstring);
        break;

    case EMON_HEATING_STATE_KEY:
        // App Sync keeps new_tuple in sync_buffer, so we may use it directly
        text_layer_set_text(text_layer, new_tuple->value->cstring);
        break;

    case EMON_FEED_COUNT_KEY:
      feedCount = (int) new_tuple->value;
  }
}

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

// Config

static void click_config_provider(void* context) {
  const uint16_t repeat_interval_ms = 100;
  window_set_click_context(BUTTON_ID_UP, context);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, up_click_handler);

  window_set_click_context(BUTTON_ID_SELECT, context);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);

  window_set_click_context(BUTTON_ID_DOWN, context);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, down_click_handler);
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {

  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);

  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);


  Tuplet initial_values[] = {
    TupletCString(EMON_TEMPERATURE_KEY, "---\u00B0C"),
  };

  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, sync_error_callback, NULL);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
