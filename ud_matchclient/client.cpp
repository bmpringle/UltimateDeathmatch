#include "client.h"
#include "input.h"
#include "emscripten/em_macros.h"
#include <ctime>
#include <emscripten/html5_webgl.h>
#include <GLES3/gl3.h>
#include <iostream>
#include "VertexTypes.h"
#include "graphics_math.h"
#include <vector>
#include <functional>

Client* Client::game_client = nullptr;

void Client::start_client() {
	game_client = new Client();
}

Client* Client::get_client() {
	return game_client;
}

Client::Client() : graphics(Graphics("#game_canvas")), camera(Camera(0.1, 10, 90)) {
	// Temporary graphics
	std::vector<Vertex3D> vertex_data = {
		Vertex3D(0, 0, 6.0),
		Vertex3D(0.5, 0, 6.0),
		Vertex3D(0.5, 0.5, 6.0),
		Vertex3D(0, 0, 6.0),
		Vertex3D(0.5, 0.5, 6.0),
		Vertex3D(0, 0.5, 6.0),

		Vertex3D(0, 0, 6.5),
		Vertex3D(0.5, 0, 6.5),
		Vertex3D(0.5, 0.5, 6.5),
		Vertex3D(0, 0, 6.5),
		Vertex3D(0.5, 0.5, 6.5),
		Vertex3D(0, 0.5, 6.5),

		Vertex3D(0, 0, 6.5),
		Vertex3D(0, 0, 6.0),
		Vertex3D(0, 0.5, 6.0),
		Vertex3D(0, 0, 6.5),
		Vertex3D(0, 0.5, 6.0),
		Vertex3D(0, 0.5, 6.5),

		Vertex3D(0.5, 0, 6.5),
		Vertex3D(0.5, 0, 6.0),
		Vertex3D(0.5, 0.5, 6.0),
		Vertex3D(0.5, 0, 6.5),
		Vertex3D(0.5, 0.5, 6.0),
		Vertex3D(0.5, 0.5, 6.5),
	};
	graphics.create_vertex_buffer(
		"triangle",
		vertex_data.data(),
		vertex_data.size(),
		GL_STATIC_DRAW
	);
	graphics.create_program("3d_shader", "resources/vertex3d.shader", "resources/fragment3d.shader");
	graphics.enable_standard_depth_testing();
}

void Client::main_loop() {
	time_since_last_clock = std::chrono::high_resolution_clock::now() - current_time;
	current_time = std::chrono::high_resolution_clock::now();
	handle_input();

	// Render
	graphics.set_active_program("3d_shader");
	GLuint vpMatrixLocation = graphics.get_uniform_buffer_location("3d_shader", "vpMatrix");
	glUniformMatrix4fv(vpMatrixLocation, 1, true, camera.get_vp_matrix().data);
	graphics.clear_screen();
	graphics.draw_buffer("triangle", {"vpMatrix"});
}

void Client::handle_mousemove_event(int eventType, const EmscriptenMouseEvent *e) {
	camera.rotation[1] -= 0.004 * (e->movementX);
	camera.rotation[0] -= 0.004 * (e->movementY);
	camera.rotation[0] = fmod(camera.rotation[0], 2.0f * M_PI);
	camera.rotation[1] = fmod(camera.rotation[1], 2.0f * M_PI);
}

void Client::handle_input() {
	std::cout << "Start" << std::endl;
	float speed = 0.5f;
	float seconds = std::chrono::duration_cast<std::chrono::microseconds>(time_since_last_clock).count() / 1000000.0;
	gmath::matrix camera_direction = camera.get_look_direction();
	gmath::matrix up_row_vector {{0, 1, 0}};

	camera_direction.cross(up_row_vector);

	if(activated_keys_map["a"]) {
		camera_direction = camera_direction.cross(up_row_vector).normalized();
	}else if(activated_keys_map["s"]) {
		camera_direction = -camera_direction;
	}else if(activated_keys_map["d"]) {
		camera_direction = -camera_direction.cross(up_row_vector).normalized();
	}

	if(activated_keys_map[" "]) {
		std::cout << "Up" << std::endl;
		camera.y += speed * seconds;
	}
	if(activated_keys_map["Shift"]) {
		std::cout << "Down" << std::endl;
		camera.y -= speed * seconds;
	}
	if(activated_keys_map["w"] || activated_keys_map["a"] || activated_keys_map["s"] || activated_keys_map["d"]) {
		std::cout << "Horizontal" << std::endl;
		camera.x += camera_direction[0, 0] * (speed * seconds);
		camera.y += camera_direction[0, 1] * (speed * seconds);
		camera.z += camera_direction[0, 2] * (speed * seconds);
	}
}

void Client::set_key_value(std::string key, bool value) {
	activated_keys_map[key] = value;
}

extern "C" void EMSCRIPTEN_KEEPALIVE resize_gl_viewport(float width, float height) {
	glViewport(0, 0, width, height);
}
