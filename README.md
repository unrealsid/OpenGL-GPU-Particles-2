# OpenGL GPU Particles

## Overview

This project implements a particle system using OpenGL with GPU acceleration.

## Project Structure

```
/OpenGL-GPU-Particles-2
├── /ext                        # External dependencies
│   ├── /glfw-3.4.bin.WIN64     # Or any higher version
│   ├── /glm
│   ├── /stb-master          
│   └── /imgui-master           # (To be added manually if needed)
├── /OpenGL_Particles           # Source code
│   ├── /resources              # Textures, models, etc.
│   ├── /shaders                # GLSL shader files
│   ├── Config.h                # Auto-generated config header
│   └── Main.cpp                # Main entry point
├── CMakeLists.txt              # Root CMake script
└── config.h.in                 # Template for Config.h
```

## CMake Setup

### Minimum Requirements

- CMake 3.8+
- Visual Studio 2022
- Windows SDK 10.0.22621.0+

### Project Configuration

To configure and generate Visual Studio project files:

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
```

This generates a Visual Studio solution `OpenGL_Particles.sln`.

### Building

To build the project after generating the solution:

- Open the generated `.sln` file in Visual Studio and build the `OpenGL_Particles` target.

## Dependency Setup

### 1. Download and Setup External Dependencies in ext folder

- **Create an `ext` directory** in the root project if it does not exist.
- Download and extract the following dependencies into the `ext` directory:
  - [GLFW](https://www.glfw.org/download.html) – Extract to `ext/glfw-3.4.bin.WIN64`
  - [GLM](https://github.com/g-truc/glm) – Extract to `ext/glm`
  - [stb](https://github.com/nothings/stb) – Extract to `ext/stb-master`
  - (Optional) [ImGui](https://github.com/ocornut/imgui) – Extract to `ext/imgui-master` if required

### Main CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.8)
project("OpenGL_Particles")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define paths with safe conversion
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL_Particles/resources/" RESOURCE_PATH)
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL_Particles/shaders/" SHADER_PATH)

# Generate Config.h
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL_Particles/Config.h
)

# Define source directory
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL_Particles)

# Get all source files
file(GLOB_RECURSE SRC_FILES
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.h"
    "${SRC_DIR}/*.glsl"
    "${SRC_DIR}/*.png"
)

# Add executable
add_executable(OpenGL_Particles ${SRC_FILES})

# Include paths
set(EXT_DIR ${CMAKE_SOURCE_DIR}/ext)
target_include_directories(OpenGL_Particles PUBLIC
    ${EXT_DIR}/glfw-3.4.bin.WIN64/include
    ${EXT_DIR}/glm
    ${EXT_DIR}/stb-master
)

# Link GLFW
if (MSVC)
    target_link_libraries(OpenGL_Particles ${EXT_DIR}/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.lib)
elseif (MINGW)
    target_link_libraries(OpenGL_Particles ${EXT_DIR}/glfw-3.4.bin.WIN64/lib-mingw-w64/libglfw3.a)
endif()

# Add glad
add_library(glad STATIC ${SRC_DIR}/glad/glad.c)
target_include_directories(OpenGL_Particles PUBLIC ${SRC_DIR}/glad)
target_link_libraries(OpenGL_Particles glad)

# Find and link OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(OpenGL_Particles OpenGL::GL)
```

## Config.h.in

`Config.h.in` is a template used to generate `Config.h` during CMake configuration. 

```c
#pragma once

// Define paths for resources and shaders
#define RESOURCE_PATH "@RESOURCE_PATH@"
#define SHADER_PATH "@SHADER_PATH@"
```

- `RESOURCE_PATH` and `SHADER_PATH` are defined dynamically based on the CMake configuration.
- CMake automatically replaces these placeholders with actual paths.

## Run Instructions

### Run after CMake Configuration

1. Configure and generate the project using CMake.
2. Build the `OpenGL_Particles` target.
3. Run the generated executable 

## Troubleshooting

### Common Issues

- **Path Issues**: Paths with spaces should be handled correctly with `file(TO_CMAKE_PATH)`.
- **Unresolved Symbols**: Check that `glfw`, `imgui`, `glad`, and `OpenGL::GL` are correctly linked.
- **Shader Path Issues**: If shaders are not found, double-check the paths configured in `Config.h`.

