#version 300 es

layout (location = 0) in vec3 pos;

out vec3 colorIn;

uniform mat4x4 vpMatrix;

void main() {
	vec4 ps = vec4(pos.xyz, 1.0) * vpMatrix;
	colorIn = vec3(0.0f, 0.0f, 0.0f);
	if(ps.x >= ps.y && ps.x >= ps.z) {
		colorIn[0] = 255.0f;
	}
	if(ps.y >= ps.x && ps.y >= ps.z) {
		colorIn[1] = 255.0f;
	}
	if(ps.z >= ps.x && ps.z >= ps.y) {
		colorIn[2] = 255.0f;
	}
	gl_Position = ps;
}
