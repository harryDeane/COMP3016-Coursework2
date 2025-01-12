#version 460 core

in vec3 FragPos;    // Position of the fragment in world space
in vec3 Normal;     // Normal vector from vertex shader
in vec2 TexCoords;  // Texture coordinates from vertex shader

out vec4 FragColor; // The output color of the fragment

uniform vec3 viewPos;       // Camera position
uniform vec3 lightPos;      // Light source position
uniform vec3 lightColor;    // Light color
uniform vec3 ambientColor;  // Ambient light color
uniform float dayTime;      // Time of day (0.0 to 1.0)

uniform sampler2D texture1; // Texture sampler for textured models
uniform bool useTexture;    // Boolean to indicate whether to use texture

// Function to calculate diffuse lighting
vec3 calculateDiffuse(vec3 normal, vec3 lightDir) {
    float diff = max(dot(normal, lightDir), 0.0);
    return diff * lightColor;
}

// Function to calculate ambient lighting
vec3 calculateAmbient() {
    return ambientColor;
}

void main() {
    // Terrain height-based color mapping
    float height = FragPos.y;
    vec4 lowColor = vec4(0.0, 0.5, 0.0, 1.0);       // Green for low-altitude areas
    vec4 midColor = vec4(0.55, 0.27, 0.07, 1.0);    // Brown for mid-altitude areas
    vec4 highColor = vec4(1.0, 1.0, 1.0, 1.0);      // White for high-altitude areas

    vec4 terrainColor;
    if (height < 1.0) {
        terrainColor = lowColor;
    } else if (height < 3.0) {
        terrainColor = mix(midColor, highColor, (height - 2.0) / 3.0); // Smooth transition
    } else {
        terrainColor = highColor;
    }

    // Lighting calculations
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 diffuse = calculateDiffuse(Normal, lightDir);

    float lightIntensity = (sin(dayTime * 3.14159f) + 1.0f) / 2.0f;
    vec3 finalAmbientColor = mix(vec3(0.1), ambientColor, lightIntensity);
    vec3 lighting = finalAmbientColor + diffuse * lightIntensity;

    // Texture sampling for models
    vec4 textureColor = texture(texture1, TexCoords);

    // Determine final color
    vec4 finalColor = useTexture ? textureColor : terrainColor;
    finalColor.rgb *= lighting; // Apply lighting

    FragColor = finalColor;
}
