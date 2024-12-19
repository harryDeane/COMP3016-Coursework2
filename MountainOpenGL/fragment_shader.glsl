#version 460 core

in vec3 FragPos;    // The position of the fragment in world space, passed from the vertex shader
in vec3 Normal;     // Normal vector passed from vertex shader
in vec2 TexCoords;  // Texture coordinates passed from vertex shader

out vec4 FragColor; // The output color of the fragment

uniform vec3 viewPos;       // Camera position
uniform vec3 lightPos;      // Light source position
uniform vec3 lightColor;    // Light color
uniform vec3 ambientColor;  // Ambient light color
uniform float dayTime;      // Time of day (0.0 to 1.0)

uniform sampler2D texture1; // Texture sampler (for additional textures if needed)

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
    // Height-based color mapping
    float height = FragPos.y;
    vec4 lowColor = vec4(0.0, 0.5, 0.0, 1.0);       // Green for low-altitude areas
    vec4 midColor = vec4(0.55, 0.27, 0.07, 1.0);    // Brown for mid-altitude areas
    vec4 highColor = vec4(1.0, 1.0, 1.0, 1.0);      // White for high-altitude areas

    vec4 terrainColor;  // The final color based on terrain height

    if (height < 2.0) {
        terrainColor = lowColor;
    } else if (height < 5.0) {
        terrainColor = mix(midColor, highColor, (height - 2.0) / 3.0); // Smooth transition
    } else {
        terrainColor = highColor;
    }

    // Lighting effect based on the day/night cycle
    float ambientStrength = 0.2f;
    vec3 baseAmbientColor = vec3(0.1f, 0.1f, 0.1f); // Base ambient color

    // Use dayTime to control the intensity of sunlight and ambient light
    float lightIntensity = (sin(dayTime * 3.14159f) + 1.0f) / 2.0f; // 0 at night, 1 at midday
    lightIntensity = lightIntensity * 1.5f; // Boost the brightness further for daytime (midday is now much brighter)

    // Diffuse lighting calculation (simulating sunlight)
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 diffuse = calculateDiffuse(Normal, lightDir);

    // Calculate final color blending ambient and diffuse light based on the time of day
    vec3 finalAmbientColor = mix(baseAmbientColor, ambientColor, lightIntensity);
    vec3 lighting = finalAmbientColor + diffuse * lightIntensity;

    // Interpolate the final terrain color with the light effect
    vec3 finalColor = terrainColor.rgb * lighting;

    // Output the final color
    FragColor = vec4(finalColor, 1.0f);
}
