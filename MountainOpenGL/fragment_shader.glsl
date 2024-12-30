#version 460 core

in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;      // Light source position
uniform vec3 lightColor;    // Light color
uniform vec3 ambientColor;  // Ambient light color

// Function to calculate diffuse lighting
vec3 calculateDiffuse(vec3 lightDir) {
    float diff = max(dot(normalize(lightDir), vec3(0, 1, 0)), 0.0); // Assuming y-up normals for now
    return diff * lightColor;
}

void main() {
    float height = FragPos.y;
    vec4 lowColor = vec4(0.0, 0.5, 0.0, 1.0);       
    vec4 midColor = vec4(0.55, 0.27, 0.07, 1.0);    
    vec4 highColor = vec4(1.0, 1.0, 1.0, 1.0);      

    vec4 terrainColor = (height < 2.0) ? lowColor : (height < 5.0) 
        ? mix(midColor, highColor, (height - 2.0) / 3.0) : highColor;

    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 diffuse = calculateDiffuse(lightDir);
    vec3 finalColor = terrainColor.rgb * (ambientColor + diffuse);
    FragColor = vec4(finalColor, 1.0f);
}

