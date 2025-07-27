#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "emscripten/em_macros.h"
#include <GLES2/gl2.h>
#include <cstdio>
#include <emscripten.h>
#include <emscripten/html5_webgl.h>
#include <GLES3/gl3.h>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <type_traits>
#include "utils.h"

struct VertexAttributeData {
	GLuint index;
	GLuint length;
	GLenum type;
	GLboolean normalized;
	GLuint stride;
	void* offset;
};

template<class T>
concept HasStaticVertexMethods = requires {
    { T::get_vertex_attributes() } -> std::same_as<std::vector<VertexAttributeData>>;
    { T::get_size() } -> std::same_as<size_t>;
};

class Graphics {
	public:
		Graphics(std::string canvas_location_css) {
			// Initialize WebGL context.
			EmscriptenWebGLContextAttributes context_attributes {};
			context_attributes.powerPreference = EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
			context_attributes.majorVersion = 2;
			context_attributes.minorVersion = 0;
			context_attributes.depth = true;
			context = emscripten_webgl_create_context(canvas_location_css.data(), &context_attributes);
			emscripten_webgl_make_context_current(context);
		}

		GLuint get_uniform_buffer_location(std::string vb_name, std::string uniform_name) {
			return glGetUniformLocation(shader_programs_map[vb_name], uniform_name.data());
		}

		template<HasStaticVertexMethods VertexData>
		void create_vertex_buffer(std::string name, VertexData* data, uint data_length, GLenum buffer_usage) {
			if (data == nullptr) {
				throw std::runtime_error("Data cannot be nullptr when creating the vertex buffer.");
			}
			glGenBuffers(1, &buffer_map[name]);
			vertex_attributes_map[name] = data[0].get_vertex_attributes();
			set_vertex_buffer_data(name, data, data_length, buffer_usage);
		}

		void create_program(std::string program_name, std::string vertex_shader_src, std::string fragment_shader_src) {
			create_shader_program(program_name, load_file(vertex_shader_src), load_file(fragment_shader_src));
		}

		void enable_standard_depth_testing() {
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_TRUE);
		}

		template<HasStaticVertexMethods VertexData>
		void set_vertex_buffer_data(std::string name, VertexData* data, uint data_length, GLenum buffer_usage) {
			if (data == nullptr) {
				throw std::runtime_error("Data cannot be nullptr when setting the vertex buffer data.");
			}
			set_buffer_data(name, GL_ARRAY_BUFFER, buffer_usage, data, data_length * data[0].get_size());
			buffer_vertex_count_map[name] = data_length;
		}

		void set_gl_clear_color(float r, float g, float b, float a) {
			glClearColor(r, g, b, a);
		}

		void clear_screen() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void set_active_program(std::string program_name) {
			glUseProgram(shader_programs_map[program_name]);
		}

		void draw_buffer(std::string vertex_buffer_name, std::vector<std::string> uniform_buffers) {
			glBindBuffer(GL_ARRAY_BUFFER, buffer_map[vertex_buffer_name]);
			set_and_enable_vertex_attributes(vertex_buffer_name);
			for(int i = 0; i < uniform_buffers.size(); ++i) {
				glBindBufferRange(GL_UNIFORM_BUFFER, i, buffer_map[uniform_buffers[i]], 0, uniform_buffer_size_map[uniform_buffers[i]]);
			}
			glDrawArrays(GL_TRIANGLES, 0, buffer_vertex_count_map[vertex_buffer_name]);
		}
	private:
		EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context;
		std::unordered_map<std::string, GLuint> buffer_map;
		std::unordered_map<std::string, GLuint> shader_programs_map;
		std::unordered_map<std::string, std::vector<VertexAttributeData>> vertex_attributes_map;
		std::unordered_map<std::string, int> buffer_vertex_count_map;
		std::unordered_map<std::string, int> uniform_buffer_size_map;

		void set_buffer_data(std::string buffer, GLenum buffer_type, GLenum usage, void* data, size_t size) {
			glBindBuffer(buffer_type, buffer_map[buffer]);
			glBufferData(buffer_type, size, data, usage);
		}

		void create_shader_program(std::string name, std::string vertex_shader_src, std::string fragment_shader_src) {
			// Create vertex shader
			GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			const char* raw_vs_str = vertex_shader_src.data();
			GLsizei vs_length = vertex_shader_src.length();
			glShaderSource(vertex_shader, 1, &raw_vs_str, &vs_length);
			glCompileShader(vertex_shader);

			// Create fragment shader
			GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
			const char* raw_fs_str = fragment_shader_src.data();
			GLsizei fs_length = fragment_shader_src.length();
			glShaderSource(fragment_shader, 1, &raw_fs_str, &fs_length);
			glCompileShader(fragment_shader);

			// Create, link, and use shader program.
			GLuint shader_program = glCreateProgram();
			glAttachShader(shader_program, vertex_shader);
			glAttachShader(shader_program, fragment_shader);
			glLinkProgram(shader_program);

			shader_programs_map[name] = shader_program;
		}

		void set_and_enable_vertex_attributes(std::string name) {
			for (VertexAttributeData attribute_data : vertex_attributes_map[name]) {
				glVertexAttribPointer(attribute_data.index, attribute_data.length, attribute_data.type, attribute_data.normalized, attribute_data.stride, attribute_data.offset);
				glEnableVertexAttribArray(attribute_data.index);
			}
		}
};

#endif
