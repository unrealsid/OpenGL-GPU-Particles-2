#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec4 ParticleColor;

struct Particle {
    vec4 position;   // xyz = position, w = size
    vec4 color;      // rgba = color
    vec4 velocity;   // xyz = velocity, w = lifetime
};

layout(std430, binding = 0) buffer ParticleBuffer 
{
    Particle particles[];
};

uniform int currentFrame; //Flipbook frame

//TODO: Change this to be a uniform
const ivec2 gridSize = ivec2(5, 5);  // Spritesheet of 5 rows and columns

uniform mat4 viewProjMatrix;
uniform mat4 viewMatrix;

void main() 
{
    Particle particle = particles[gl_InstanceID];
    vec3 particlePos = particle.position.xyz;
    float particleSize = particle.position.w;

    // Billboard calculation
    vec3 cameraRight = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
    vec3 cameraUp = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

    // Create the billboard
    vec3 vertexPosition = particlePos
        + cameraRight * aPos.x * particleSize
        + cameraUp * aPos.y * particleSize;
    
    // Sprite sheet animation calculation
    // Use particle lifetime to determine sprite frame
    float lifetime = particle.velocity.w;
    float maxLifetime = 3.0; // Match the max lifetime in compute shader

    // Calculate sprite index based on lifetime
    int totalSprites = gridSize.x * gridSize.y;
    int currentSprite = int(mod(floor(lifetime / maxLifetime * float(totalSprites)), float(totalSprites)));

    // Calculate sprite sheet UV offset
    int spriteX = currentSprite % gridSize.y;
    int spriteY = currentSprite / gridSize.y;

    // Adjust texture coordinates for sprite sheet
    vec2 baseTex = aTexCoord;
    TexCoord = vec2
    (
        (baseTex.x + float(spriteX)) / float(gridSize.y),
        (baseTex.y + float(spriteY)) / float(gridSize.x)
    );
    
    ParticleColor = particle.color;

    gl_Position = viewProjMatrix * vec4(vertexPosition, 1.0);
}