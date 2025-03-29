#pragma once

#include "../glad/glad.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <random>

namespace particle_simulation
{
    struct Particle
    {
        glm::vec4 position;   // xyz = position, w = size
        glm::vec4 color;      // rgba = color
        glm::vec4 velocity;   // xyz = velocity, w = lifetime
    };

    class ParticleSimulation
    {
    public:
        ParticleSimulation(int maxParticles, const glm::vec3& emitterLocation);
        ~ParticleSimulation();
    
        void init();
        void update(double deltaTime);
        void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

        void PauseSim();

        void cleanup();
        void destroy();
    
    private:
        void createParticles();
    
        int maxParticles;
        GLuint particleBuffer;
        GLuint renderVAO;
        GLuint billboardVBO;
    
        GLuint renderProgram;
        GLuint computeProgram;
    
        GLuint smokeTexture;
    
        // Uniform locations
        GLuint viewProjMatrixLocation;
        GLuint deltaTimeLocation;
        GLuint viewMatrixLocation;
    
        // Random number generator
        std::mt19937 rng;
        std::uniform_real_distribution<float> dist;

        glm::vec3 previousEmitterLocation;
        glm::vec3 currentEmitterLocation;

        bool bPause;
        
        float frameRate;
        float maxParticleLifetime;
        int totalFrames;
    };
}
