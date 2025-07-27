#version 300 es

precision mediump float;

in vec3 colorIn;
out vec4 color;

void main() {
	color = vec4(colorIn.xyz, 1.0f);
}
