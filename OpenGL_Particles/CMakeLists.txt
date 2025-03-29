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

# Define the resources directory (relative to the project root)
set(RESOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/resources" CACHE STRING "Path to resources directory")

# Configure a header file with this path
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in  # Template file
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL_Particles/Config.h      # Output file
)

# Define the source directory
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL_Particles)

# Automatically get all .cpp and .h files from the src directory
file(GLOB_RECURSE SRC_FILES
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.h"
)

add_executable(OpenGL_Particles ${SRC_FILES})

# Include the generated file in the project
include_directories(${CMAKE_BINARY_DIR})
