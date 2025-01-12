// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define STB_PERLIN_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include "stb_perlin.h"
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Camera.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"

#include <stdio.h>
#include <irrKlang.h>

using namespace irrklang;

#pragma comment(lib, "irrKlang.lib")

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

GLuint VAO, VBO, EBO, textureID;

GLuint loadTexture(const char* path) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
        return 0;
    }

    std::cerr << "texture loaded: " << path << std::endl;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    stbi_image_free(data);

    return texture;
}


// Load Model
void loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
        return;
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    aiMesh* mesh = scene->mMeshes[0];
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        vertex.texCoords = { mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][i].x : 0.0f,
                             mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][i].y : 0.0f };
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Set up buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// Render Model
void renderModel(GLuint shaderProgram) {
    glUseProgram(shaderProgram);

    // Set texture uniform
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1); // Model uses texture
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Render the model
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


// Terrain generation settings
const int gridSize = 100;  // The size of the terrain grid
const float scale = 1.0f;  // Controls the spacing between vertices
const float frequency = 0.1f; // Controls the "roughness" of the terrain
const float amplitude = 10.0f; // Controls the height of the terrain

// Time tracking for day/night cycle
float dayTime = 0.0f;
const float dayCycleSpeed = 0.1f; // Adjust to control how fast the day passes


// Global camera object
Camera camera(glm::vec3(0.0f, 1.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

// Time tracking variables
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f;

// Mouse movement callback function
float lastX = 400, lastY = 300;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // Invert Y axis
    lastX = xpos;
    lastY = ypos;

    std::cout << "Mouse Offsets - X: " << xoffset << ", Y: " << yoffset << std::endl;

    camera.ProcessMouseMovement(xoffset, yoffset, true); // or false depending on your need
}

// Callback for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Handle user input
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Pass window context to camera
    camera.ProcessKeyboard(window, deltaTime);
}

// Function to generate terrain vertices and indices
void generateTerrain(std::vector<float>& vertices, std::vector<unsigned int>& indices, int gridSize, float scale) {
    for (int z = 0; z <= gridSize; ++z) {
        for (int x = 0; x <= gridSize; ++x) {
            // Use Perlin noise to generate height
            float height = stb_perlin_noise3(x * frequency, z * frequency, 0.0f, 0, 0, 0) * amplitude;
            vertices.push_back(x * scale);  // X coordinate
            vertices.push_back(height);    // Y (height)
            vertices.push_back(z * scale); // Z coordinate
        }
    }

    // Generate indices for rendering triangles
    for (int z = 0; z < gridSize; ++z) {
        for (int x = 0; x < gridSize; ++x) {
            int topLeft = z * (gridSize + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (gridSize + 1) + x;
            int bottomRight = bottomLeft + 1;

            // Create two triangles per square
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

void renderTerrain(GLuint shaderProgram) {

    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 0);

    // Generate terrain data
    std::vector<float> terrainVertices;
    std::vector<unsigned int> terrainIndices;
    generateTerrain(terrainVertices, terrainIndices, gridSize, scale);

    // Create VAO, VBO, and EBO for terrain
    GLuint terrainVAO, terrainVBO, terrainEBO;
    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);
    glGenBuffers(1, &terrainEBO);

    glBindVertexArray(terrainVAO);

    // Vertex buffer for terrain
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, terrainVertices.size() * sizeof(float), terrainVertices.data(), GL_STATIC_DRAW);

    // Element buffer for terrain
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrainIndices.size() * sizeof(unsigned int), terrainIndices.data(), GL_STATIC_DRAW);

    // Vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glBindVertexArray(terrainVAO);
    glDrawElements(GL_TRIANGLES, terrainIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Function to read shader source from file
std::string loadShaderSource(const std::string& filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        exit(EXIT_FAILURE);
    }

    std::stringstream buffer;
    buffer << shaderFile.rdbuf();
    return buffer.str();
}

// Function to compile a shader
GLuint compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader Compilation Error:\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSource = loadShaderSource(vertexPath);
    std::string fragmentSource = loadShaderSource(fragmentPath);

    GLuint vertexShader = compileShader(vertexSource.c_str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource.c_str(), GL_FRAGMENT_SHADER);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader Linking Error:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Add light properties
glm::vec3 lightPos(50.0f, 50.0f, 50.0f);  // Directional light position
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);   // White light (Sunlight)
glm::vec3 ambientColor(0.1f, 0.1f, 0.1f);  // Low ambient light to simulate dawn/dusk

// Function to load a model using Assimp
GLuint loadModel(const std::string& path, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return 0;
    }

    aiMesh* mesh = scene->mMeshes[0];  // Assuming the first mesh in the scene for simplicity

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        aiVector3D vertex = mesh->mVertices[i];
        vertices.push_back(vertex.x);
        vertices.push_back(vertex.y);
        vertices.push_back(vertex.z);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return VAO;
}

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the sound engine
    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
    if (!engine) {
        std::cerr << "Could not create sound engine!" << std::endl;
        return -1;
    }

    // Create a window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Mountain Landscape Explorer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Load background music and play it in a loop
    irrklang::ISound* backgroundMusic = engine->play2D("sounds/background.wav", true, false, true);


    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Set the mouse callback function
    glfwSetCursorPosCallback(window, mouse_callback);

    // Enable mouse capture mode for first-person view
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Create shader program
    GLuint shaderProgram = createShaderProgram("vertex_shader.glsl", "fragment_shader.glsl");
    glUseProgram(shaderProgram);

    // Set uniform values for the light
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(shaderProgram, "ambientColor"), 1, glm::value_ptr(ambientColor));

    // Create transformation matrices
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // Set shader uniform values
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


    // Load model and texture
    loadModel("models/block.obj");
    textureID = loadTexture("models/target.jpg");

    // Main render loop
    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Update dayTime before updating shader uniform
        dayTime += deltaTime * dayCycleSpeed;
        if (dayTime > 1.0f) {
            dayTime = 0.0f; // Reset to 0 when a full cycle is completed
        }

        // Update the dayTime uniform
        glUniform1f(glGetUniformLocation(shaderProgram, "dayTime"), dayTime);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update the view matrix based on camera movement
        view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        renderTerrain(shaderProgram);

        // Render the model
        renderModel(shaderProgram);

        // Keep the music looping
        if (backgroundMusic && backgroundMusic->isFinished()) {
            backgroundMusic->setIsLooped(true);  // Loop the music if it's finished
        }

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();

        glGetError();
    }

    engine->drop();  // Release the sound engine
    glfwTerminate();
    return 0;
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
