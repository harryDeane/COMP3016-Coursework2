# OpenGL Mountain Landscape Explorer

## Overview
This project is an interactive 3D mountain landscape explorer built using OpenGL and various other libraries like GLFW, GLEW, ASSIMP, and Irrklang. It features terrain generation, model and texture loading, audio playback, camera movement, and more. The goal of the project is to provide an immersive environment where the player navigates a procedurally generated mountain landscape.

## Features

### 1. **Terrain Generation**
The terrain is generated using **Perlin noise** to create a natural-looking mountain landscape. A heightmap is generated procedurally using the `stb_perlin.h` library, which is then used to create a 3D terrain mesh. The terrain adapts based on the generated noise, giving the player a unique environment to explore each time the game is run.

- **Heightmap Generation**: The `generateTerrain` function utilizes Perlin noise to calculate elevation at each point in the grid.
- **Dynamic Texturing**: Different textures are applied based on the elevation and axis of the terrain (e.g., snow on top, grass on lower levels).

### 2. **Model Loading**
The project uses **ASSIMP** (Open Asset Import Library) to load 3D models into the scene. Models can include static objects like rocks, trees, and other environmental assets.

- **Model File Formats**: Supports common file formats like `.obj` for 3D models.
- **Scene Setup**: Models are placed into the scene, and their positions, rotations, and scales are adjusted.

### 3. **Texture Loading**
Textures are applied to the terrain and 3D models to improve realism.

- **Texture Formats**: Loads texture files (e.g., `.png`, `.jpg`) using OpenGL.
- **Shader Integration**: Custom shaders are used for applying textures onto models and terrain, allowing for realistic lighting and shadow effects.

### 4. **Audio (Background Music)**
The game uses **Irrklang** to handle audio playback, with background music playing continuously throughout the game.

- **Music Looping**: Background music is loaded and played on a loop using the `engine->play2D` function.

### 5. **Camera**
The camera in the game is designed for navigation through the 3D landscape.

- **First-Person View**: The camera follows the player's viewpoint, providing a first-person perspective as the player moves through the terrain.
- **Camera Controls**: The camera is controlled using the keyboard (WASD for movement) and the mouse for rotation. This allows players to explore the environment freely.
- **View Frustum**: The camera dynamically adjusts to the terrain, ensuring that it properly displays the landscape while keeping the player centered in view.

### 6. **Movement**
Movement through the landscape is handled using keyboard input for navigation.

- **WASD Controls**: The player moves in the landscape using standard WASD controls for forward, backward, left, and right movement.
- **Mouse Look**: The mouse is used to control the camera's pitch and yaw, allowing for smooth, free movement in the environment.

### 7. **Lighting**
Lighting effects are implemented to provide realistic shading in the 3D world.

- **Directional Light**: A moving sunlight source illuminates the terrain and models, casting realistic shadows.
- **Ambient Lighting**: An ambient light source ensures the environment is not completely dark in shadowed areas, providing a balanced lighting scheme.

### 8. **Shaders**
Custom shaders are used to enhance the visual quality of the game.

- **Vertex Shader**: Transforms vertices from 3D space to screen space.
- **Fragment Shader**: Handles the coloring and texturing of the terrain and models.

---

## Installation

1. **Clone the Repository**:

    ```bash
    git clone https://github.com/yourusername/MountainLandscapeExplorer.git
    cd MountainLandscapeExplorer
    ```

2. **Dependencies**:
    Ensure you have the following libraries installed:
    - **GLFW**: For window creation and input handling.
    - **GLEW**: For OpenGL function loading.
    - **ASSIMP**: For model loading.
    - **Irrklang**: For audio playback.
    - **stb_perlin.h**: For procedural terrain generation.

3. **Build**:
    Use your preferred IDE (e.g., Visual Studio) or CMake to build the project. Make sure all libraries are linked correctly.

4. **Run**:
    Execute the built application to start exploring the 3D mountain landscape!

---

## Future Improvements
- **Physics interactions** (throwable snowballs).
- **Optimizations** for better performance with larger landscapes.

---
