#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include <GL/gl.h>
#include "debug/GL_Debug.h"
#include <memory>
#include <windows.h>

#include "Config.h"
#include "systems/ParticleSystem.h"

//Target NVIDIA cards
extern "C" 
{
    _declspec(dllexport) DWORD NvOptimusEnablement = 1;
    _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0;
}

namespace
{
    float aspectRatio = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
    float fov = 45.0f;
    
    std::unique_ptr<particle_simulation::ParticleSimulation> fireParticleSimulation = nullptr;

    std::unique_ptr<particle_simulation::ParticleSimulation> smokeParticleSimulation = nullptr;
    
    void initScene()
    {
        //Initialize the particle system and call the init method on it
        fireParticleSimulation = std::make_unique<particle_simulation::ParticleSimulation>(
            2000,
            glm::vec3(0.0f, -1.0f, 0.0f),
            25,
            glm::ivec2(5, 5),
            60.0,
            5.0, 0.5, "fireSheet5x5_alpha.png");
        
        fireParticleSimulation->init();

        smokeParticleSimulation = std::make_unique<particle_simulation::ParticleSimulation>(
            500,
            glm::vec3(0.0f, -1.0f, 0.0f),
            25,
            glm::ivec2(5, 5),
            30.0,
            4.0, 1.0, "smoke_sheet.png");

        smokeParticleSimulation->init();
    }
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // Set OpenGL version to 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    //Enable debugging
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Set the OpenGL profile to core (modern OpenGL)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Fire Effect", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the OpenGL context current
    glfwMakeContextCurrent(window);

    // Initialize GLAD (after setting the OpenGL context)
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    //Init OpenGL debug function to capture debug messages
    enableOpenGLDebug();

    //Init the scene
    initScene();

    double lastTime = glfwGetTime();  // Store the time at the start
    double deltaTime = 0.0;  // Time between frames

    // Print the OpenGL version to confirm it's 4.6
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "OpenGL version: " << version << std::endl;

    // Set up camera
    glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt
    (
        glm::vec3(0.0f, 6.0f, 5.0f),   // Camera position
        glm::vec3(0.0f, 1.0f, 0.0f),   // Look at
        glm::vec3(0.0f, 1.0f, 0.0f)    // Up vector
    );
    
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;

        lastTime = currentTime;
        
        fireParticleSimulation->update(deltaTime);
        smokeParticleSimulation->update(deltaTime);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        
        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        particle_simulation::ParticleSimulation::beginBlend();       
        
        // Render smoke
        fireParticleSimulation->render(view, projection);
        smokeParticleSimulation->render(view, projection);

        particle_simulation::ParticleSimulation::endBlend();

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            //TODO: Add interaction
        }
        
        // Swap buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Clean up and terminate
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}