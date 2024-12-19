#include "Camera.h"
#include <C:/Users/harry/Desktop/COMP3016/Part2/test/MountainOpenGL/glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), speed(5.0f), mouseSensitivity(0.2f), worldUp(up), yaw(yaw), pitch(pitch) {

    this->position = position;
    right = glm::normalize(glm::cross(front, worldUp));  // Calculate right vector
    this->up = glm::normalize(glm::cross(right, front)); // Recalculate up vector
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetProjectionMatrix(float screenWidth, float screenHeight) {
    return glm::perspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 100.0f);
}

void Camera::ProcessKeyboard(GLFWwindow* window, float deltaTime) {
    float velocity = speed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= front * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * velocity;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += up * velocity;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        position -= up * velocity;
}


void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Constrain pitch to avoid flipping the camera
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // Update camera vectors
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    // Calculate new front vector
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front); // Normalize the front vector

    // Recalculate the right and up vectors
    right = glm::normalize(glm::cross(front, worldUp)); // Right vector
    up = glm::normalize(glm::cross(right, front)); // Up vector
}

