#include "emscripten/em_macros.h"
#include <emscripten.h>
#include <emscripten/html5_webgl.h>
#include <GLES3/gl3.h>
#include <iostream>
#include "graphics.h"
#include "VertexTypes.h"

Graphics* graphics;

void game_loop() {
	graphics->clear_screen();
	graphics->draw_buffer("triangle");
}

void graphics_init() {
	graphics = new Graphics("#game_canvas");
	Vertex2D data [3] = {
		Vertex2D(-1, -1),
		Vertex2D(0, 1),
		Vertex2D(1, -1)
	};
	graphics->create_vertex_buffer(
		"triangle",
		"#version 300 es\nlayout (location = 0) in vec2 pos;\nvoid main() { gl_Position = vec4(pos.x, pos.y, 0, 1.0);} ",
		"#version 300 es\nprecision mediump float;\nout vec4 color;\nvoid main() { color = vec4(1.0, 1.0, 1.0, 1.0);} ",
		data,
		3,
		GL_STATIC_DRAW
	);
}

extern "C" void EMSCRIPTEN_KEEPALIVE resize_gl_viewport(float width, float height) {
	glViewport(0, 0, width, height);
}

int EMSCRIPTEN_KEEPALIVE main() {
	graphics_init();
	emscripten_set_main_loop(game_loop, 60, true);
	return 0;
}
