#version 300 es

layout (location = 0) in vec2 pos;

void main() {
	gl_Position = vec4(pos.x, pos.y, 0, 1.0);
}
