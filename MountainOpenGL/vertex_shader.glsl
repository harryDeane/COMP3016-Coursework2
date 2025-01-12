#version 460 core

layout(location = 0) in vec3 aPos;     // Vertex position
layout(location = 1) in vec3 aNormal;  // Vertex normal
layout(location = 2) in vec2 aTexCoords; // Vertex texture coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;    // World position of the fragment
out vec3 Normal;     // Normal vector
out vec2 TexCoords;  // Texture coordinates

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0)); // Transform vertex to world space
    Normal = mat3(transpose(inverse(model))) * aNormal; // Correct normal for non-uniform scaling
    TexCoords = aTexCoords; // Pass texture coordinates to fragment shader

    gl_Position = projection * view * vec4(FragPos, 1.0); // Transform to clip space
}
