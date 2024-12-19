#ifndef CAMERA_H
#define CAMERA_H

#include "C:/Users/harry/Desktop/COMP3016/Part2/test/MountainOpenGL/glm/glm.hpp"
#include <C:/Users/harry/Desktop/COMP3016/Part2/test/MountainOpenGL/glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    // Get view and projection matrices
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(float screenWidth, float screenHeight);

    // Handle camera movement
    void ProcessKeyboard(GLFWwindow* window, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);

    // Setters
    void SetPosition(glm::vec3 pos) { position = pos; }
    void SetYaw(float newYaw) { yaw = newYaw; updateCameraVectors(); }
    void SetPitch(float newPitch) { pitch = newPitch; updateCameraVectors(); }

    // Getters
    glm::vec3 GetPosition() const { return position; }
    float GetYaw() const { return yaw; }
    float GetPitch() const { return pitch; }

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float speed;
    float mouseSensitivity;

    // Private method to update camera vectors
    void updateCameraVectors();
};

#endif

