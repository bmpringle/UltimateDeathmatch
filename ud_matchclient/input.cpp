#include "input.h"

bool keydown_callback(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
	std::cout << e->key << " down" << std::endl;
	Client::get_client()->set_key_value(e->key, true);
  	return false;
}

bool keyup_callback(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
	std::cout << e->key << " up" << std::endl;
	Client::get_client()->set_key_value(e->key, false);
  	return false;
}

bool mousemove_callback(int eventType, const EmscriptenMouseEvent* e, void* userData) {
	Client::get_client()->handle_mousemove_event(eventType, e);
  	return false;
}
