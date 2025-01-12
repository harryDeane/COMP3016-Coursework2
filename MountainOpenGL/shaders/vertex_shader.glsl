#version 460 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos; // Output position to fragment shader

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0)); // Transform vertex to world space
    gl_Position = projection * view * vec4(FragPos, 1.0); // Transform to clip space
}
