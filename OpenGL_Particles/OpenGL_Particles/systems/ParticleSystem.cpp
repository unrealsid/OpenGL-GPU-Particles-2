// smoke_simulation.cpp
#include "ParticleSystem.h"
#include "stb_image.h"
#include "../utilities/ShaderUtils.h"
#include "../Config.h"

particle_simulation::ParticleSimulation::ParticleSimulation(int maxParticles, const glm::vec3& emitterLocation) : maxParticles(maxParticles)
{
    rng = std::mt19937(static_cast<unsigned int>(time(nullptr)));
    dist = std::uniform_real_distribution<float>(-1.0f, 1.0f);
    currentEmitterLocation = emitterLocation;
    previousEmitterLocation = glm::vec3(0.0f);
    bPause = true;
}

particle_simulation::ParticleSimulation::~ParticleSimulation()
{
    cleanup();
}

void particle_simulation::ParticleSimulation::init()
{
    // Create and compile shaders
    renderProgram = ShaderUtils::loadShader(std::string(SHADER_PATH) + "/vertex.glsl", std::string(SHADER_PATH) + "/fragment.glsl");
    computeProgram = ShaderUtils::loadComputeShader(std::string(SHADER_PATH) + "/compute.glsl");

    // Get uniform locations
    viewProjMatrixLocation = glGetUniformLocation(renderProgram, "viewProjMatrix");
    viewMatrixLocation = glGetUniformLocation(renderProgram, "viewMatrix");
    deltaTimeLocation = glGetUniformLocation(computeProgram, "deltaTime");

    // Create particles SSBO
    glGenBuffers(1, &particleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);

    // Initialize particles
    createParticles();

    // Create billboard vertices for rendering
    static const GLfloat billboardVertices[] =
    {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &renderVAO);
    glBindVertexArray(renderVAO);

    glGenBuffers(1, &billboardVBO);
    glBindBuffer(GL_ARRAY_BUFFER, billboardVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(billboardVertices), billboardVertices, GL_STATIC_DRAW);

    // Position + UV attributes for the billboard vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // Load smoke texture
    glGenTextures(1, &smokeTexture);
    glBindTexture(GL_TEXTURE_2D, smokeTexture);

    int width, height, channels;
    unsigned char* data = stbi_load((std::string(RESOURCE_PATH) + "/fireSheet5x5_alpha.png").c_str(), &width, &height, &channels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void particle_simulation::ParticleSimulation::createParticles()
{
    std::vector<Particle> particles(maxParticles);

    glUseProgram(computeProgram);
    ShaderUtils::setUniformVec3(computeProgram, "particleEmitterOrigin", currentEmitterLocation);

    for (int i = 0; i < maxParticles; i++)
    {
        particles[i].velocity = glm::vec4(0.0f);
    }

    ShaderUtils::setUniformInt(computeProgram, "emitterAlive", 1);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, maxParticles * sizeof(Particle), particles.data());
}

void particle_simulation::ParticleSimulation::update(double deltaTime)
{
    glUseProgram(computeProgram);
    ShaderUtils::setUniformFloat(computeProgram, "deltaTime", deltaTime);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);

    ShaderUtils::setUniformVec3(computeProgram, "particleEmitterCurrentPos", currentEmitterLocation);
    ShaderUtils::setUniformVec3(computeProgram, "prevParticleEmitterPos", previousEmitterLocation);

    previousEmitterLocation = currentEmitterLocation;

    int workGroupSize = 128;
    int numGroups = (maxParticles + workGroupSize - 1) / workGroupSize;
    glDispatchCompute(numGroups, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void particle_simulation::ParticleSimulation::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    glm::mat4 viewProjMatrix = projectionMatrix * viewMatrix;

    glUseProgram(renderProgram);
    ShaderUtils::setUniformMat4(renderProgram, "viewProjMatrix", viewProjMatrix);
    ShaderUtils::setUniformMat4(renderProgram, "viewMatrix", viewMatrix);

    float frameRate = 60.0f;
    int totalFrames = 25;
    float time = glfwGetTime();
    int currentFrame = static_cast<int>(time * frameRate) % totalFrames;
    ShaderUtils::setUniformInt(renderProgram, "currentFrame", currentFrame);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, smokeTexture);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glBindVertexArray(renderVAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, maxParticles);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void particle_simulation::ParticleSimulation::PauseSim()
{
    //TODO: Change this
    bPause = !bPause;
}

void particle_simulation::ParticleSimulation::cleanup()
{
    glDeleteBuffers(1, &particleBuffer);
    glDeleteVertexArrays(1, &renderVAO);
    glDeleteBuffers(1, &billboardVBO);
    glDeleteProgram(renderProgram);
    glDeleteProgram(computeProgram);
    glDeleteTextures(1, &smokeTexture);
}

void particle_simulation::ParticleSimulation::destroy()
{
    glUseProgram(computeProgram);
    ShaderUtils::setUniformInt(computeProgram, "emitterAlive", 0);
    cleanup();
}
