#version 400 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec4 a_color;

out vec3 v_normal;
out vec3 v_fragPos;
out vec4 v_color;

void main() {
    // Just pass attributes to the next stage
    v_normal = a_normal;
    v_color = a_color;
    v_fragPos = a_position; // raw position

    gl_Position = vec4(a_position, 1.0);
}