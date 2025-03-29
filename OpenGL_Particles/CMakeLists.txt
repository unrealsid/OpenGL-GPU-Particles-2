# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("OpenGL_Particles")

# Require at least C++17 or later
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Convert paths to CMake-safe paths
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL_Particles/resources/" RESOURCE_PATH)
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL_Particles/shaders/" SHADER_PATH)

# Store them as cache variables to allow editing via CMake GUI or Visual Studio
set(RESOURCE_PATH "${RESOURCE_PATH}" CACHE STRING "Path to the resources directory")
set(SHADER_PATH "${SHADER_PATH}" CACHE STRING "Path to the shaders directory")

# Configure a header file with this path
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"  # Template file
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL_Particles/Config.h"  # Output file
)

# Define the source directory
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL_Particles)

# Automatically get all .cpp and .h files from the src directory
file(GLOB_RECURSE SRC_FILES
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.h"
	"${SRC_DIR}/*.glsl"
	"${SRC_DIR}/*.png"
)

# ------------------------------------------------------
# 1. Build the main executable FIRST
# ------------------------------------------------------
add_executable(OpenGL_Particles ${SRC_FILES})

# Include the generated file in the project
include_directories(${CMAKE_BINARY_DIR})

# Add glad.c as part of the executable
target_sources(OpenGL_Particles PUBLIC ${SRC_DIR}/glad/glad.c)


# Add dependencies directory
set(EXT_DIR ${CMAKE_SOURCE_DIR}/ext)

# ------------------------------------------------------
# 2. Include and link GLFW
# ------------------------------------------------------
target_include_directories(OpenGL_Particles PUBLIC ${EXT_DIR}/glfw-3.4.bin.WIN64/include)

# Choose the correct GLFW library based on compiler
if (MSVC)
    target_link_libraries(OpenGL_Particles ${EXT_DIR}/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.lib)
elseif (MINGW)
    target_link_libraries(OpenGL_Particles ${EXT_DIR}/glfw-3.4.bin.WIN64/lib-mingw-w64/libglfw3.a)
endif()

# ------------------------------------------------------
# 3. Add GLM
# ------------------------------------------------------
target_include_directories(OpenGL_Particles PUBLIC ${EXT_DIR}/glm)

# ------------------------------------------------------
# 4. Add ImGui
# ------------------------------------------------------
set(IMGUI_DIR ${EXT_DIR}/imgui-master)
file(GLOB IMGUI_SOURCES
    "${IMGUI_DIR}/*.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

# Add ImGui as a library
add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(OpenGL_Particles PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(OpenGL_Particles imgui)

# ------------------------------------------------------
# 4. Add stbi
# ------------------------------------------------------

target_include_directories(OpenGL_Particles PUBLIC ${EXT_DIR}/stb-master)

# ------------------------------------------------------
# 5. Add OpenGL (system library)
# ------------------------------------------------------
find_package(OpenGL REQUIRED)

# Link OpenGL to the executable
target_link_libraries(OpenGL_Particles OpenGL::GL)
