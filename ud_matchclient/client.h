#ifndef CLIENT_H
#define CLIENT_H

#include <chrono>
#include <emscripten.h>
#include "graphics.h"
#include "camera.h"
#include <unordered_map>

class Client {
	public:
		static void start_client();

		static Client* get_client();

		void handle_mousemove_event(int eventType, const EmscriptenMouseEvent *e);

		void main_loop();

		void set_key_value(std::string key, bool value);
	private:
		Client();

		Graphics graphics;
		Camera camera;
		static Client* game_client;

		std::chrono::time_point<std::chrono::high_resolution_clock> current_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_since_last_clock = current_time - current_time;
		std::unordered_map<std::string, bool> activated_keys_map;

		void handle_input();
};

extern "C" void EMSCRIPTEN_KEEPALIVE resize_gl_viewport(float width, float height);

#endif
