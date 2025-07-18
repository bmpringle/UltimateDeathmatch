#include "emscripten/em_macros.h"
#include <emscripten.h>
#include <emscripten/html5_webgl.h>
#include <GLES3/gl3.h>
#include <iostream>

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context;
GLuint vertex_buffer;

void game_loop() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void graphics_init() {
	// Initialize WebGL context.
	EmscriptenWebGLContextAttributes context_attributes;
	context_attributes.powerPreference = EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
	context_attributes.majorVersion = 2;
	context_attributes.minorVersion = 0;
	context = emscripten_webgl_create_context("#game_canvas", &context_attributes);
	emscripten_webgl_make_context_current(context);

	// Create vertex buffer.
	glGenBuffers(1, &vertex_buffer);
	float vertex_data[6] = {-1, -1, 0, 1, 1, -1};
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * 3 * sizeof(float), &vertex_data, GL_STATIC_DRAW);

	// Create vertex shader
	std::string vertex_shader_src = "#version 300 es\nlayout (location = 0) in vec2 pos;\nvoid main() { gl_Position = vec4(pos.x, pos.y, 0, 1.0);} ";
	int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char* raw_vs_str = vertex_shader_src.data();
	int vs_length = vertex_shader_src.length();
	glShaderSource(vertex_shader, 1, &raw_vs_str, &vs_length);
	glCompileShader(vertex_shader);

	// Create fragment shader
	std::string fragment_shader_src = "#version 300 es\nprecision mediump float;\nout vec4 color;\nvoid main() { color = vec4(1.0, 1.0, 1.0, 1.0);} ";
	int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* raw_fs_str = fragment_shader_src.data();
	int fs_length = fragment_shader_src.length();
	glShaderSource(fragment_shader, 1, &raw_fs_str, &fs_length);
	glCompileShader(fragment_shader);

	// Create, link, and use shader program.
	int shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glUseProgram(shader_program);

	// Delete shaders
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Bind vertex attribute data.
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

extern "C" void EMSCRIPTEN_KEEPALIVE resize_gl_viewport(float width, float height) {
	glViewport(0, 0, width, height);
}

int EMSCRIPTEN_KEEPALIVE main() {
	graphics_init();
	emscripten_set_main_loop(game_loop, 60, true);
	return 0;
}
