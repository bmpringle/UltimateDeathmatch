#include "client.h"
#include "input.h"

void __client_main_loop__() {
	Client::get_client()->main_loop();
}

int EMSCRIPTEN_KEEPALIVE main() {
	// Set input callbacks.
	emscripten_set_keydown_callback("#game_canvas", nullptr, false, keydown_callback);
	emscripten_set_keyup_callback("#game_canvas", nullptr, true, keyup_callback);
	emscripten_set_mousemove_callback("#game_canvas", nullptr, true, mousemove_callback);

	Client::start_client(); // Start client.

	emscripten_set_main_loop(__client_main_loop__, 0, true); // Set main loop.
	return 0;
}
