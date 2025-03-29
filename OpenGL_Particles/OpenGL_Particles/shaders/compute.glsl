#version 460 core

layout(local_size_x = 128) in;

struct Particle 
{
    vec4 position;   // xyz = position, w = size
    vec4 color;      // rgba = color
    vec4 velocity;   // xyz = velocity, w = lifetime
};

layout(std430, binding = 0) buffer ParticleBuffer
{
    Particle particles[];
};

uniform float deltaTime;
uniform float sphereRadius;
uniform float maxLifetime;
uniform vec3 particleEmitterCurrentPos;
uniform vec3 prevParticleEmitterPos;
uniform bool emitterAlive;

void main() 
{
    uint gid = gl_GlobalInvocationID.x;

    if (gid >= particles.length()) 
        return;

    if(emitterAlive) 
    {
        // Update lifetime
        particles[gid].velocity.w -= deltaTime;
    
        // If the particle is dead, respawn it
        if (particles[gid].velocity.w <= 0.0) 
        {
            // Generate random values for spherical coordinates
            float u = fract(sin(float(gid) * 12.9898) * 43758.5453);  // Random value u
            float v = fract(sin(float(gid) * 78.233) * 43758.5453);   // Random value v
    
            // Spherical coordinates to cartesian
            float theta = u * 2.0 * 3.14159;       // Azimuthal angle (0 to 2π)
            float phi = acos(2.0 * v - 1.0);       // Polar angle (0 to π)
            float r = sphereRadius * pow(fract(sin(float(gid) * 34.567) * 43758.5453), 1.0 / 3.0);  // Uniform distribution with cbrt
    
            // Particle position offset by current emitter location
            particles[gid].position = vec4
            (
                particleEmitterCurrentPos.x + r * sin(phi) * cos(theta),  // X coordinate
                particleEmitterCurrentPos.y + r * sin(phi) * sin(theta),  // Y coordinate
                particleEmitterCurrentPos.z + r * cos(phi),               // Z coordinate
                1.0 + fract(sin(float(gid) * 56.789) * 43758.5453) * 0.5  // Random size between 0.5 and 1.0
            );
    
            // White smoke with random alpha
            particles[gid].color = vec4
            (
                0.9, 0.9, 0.9, 
                0.5 + 0.5 * fract(sin(float(gid) * 12.7) * 43758.5453)
            );
    
            // Random upward velocity
            particles[gid].velocity = vec4
            (
                mix(-0.2, 0.2, fract(sin(float(gid) * 32.8) * 43758.5453)),
                0.5 + 0.5 * fract(sin(float(gid) * 94.2) * 43758.5453),
                mix(-0.2, 0.2, fract(sin(float(gid) * 70.3) * 43758.5453)),
                maxLifetime * fract(sin(float(gid) * 18.6) * 43758.5453) // Random lifetime upto maxLifetime
            );
        } 
        else 
        {
            // Update position based on velocity
            particles[gid].position.xyz += (particles[gid].velocity.xyz * deltaTime);
    
            // Handle base position movement
            vec3 baseDelta = particleEmitterCurrentPos - prevParticleEmitterPos;
            particles[gid].position.xyz += baseDelta;
            
            // Apply some wind effect
            particles[gid].velocity.x += 0.05 * deltaTime * sin(particles[gid].position.y * 0.5 + deltaTime * 0.2);
    
            // Age-based scaling
            float currentLifetime = particles[gid].velocity.w;
            float lifePercent = 1.0 - (currentLifetime / maxLifetime);
    
            // Increase size as the particle ages
            // Scale starts small and grows to a maximum size
            float minSize = 0.8;
            float maxSize = 2.0;
            particles[gid].position.w = mix(minSize, maxSize, lifePercent);
    
            // Decrease opacity over time
            particles[gid].color.a = (1.0 - lifePercent) * 0.7;
    
            // Add some turbulence
            float turbulenceX = sin(particles[gid].position.y * 8.0 + deltaTime) * 0.3;
            float turbulenceZ = cos(particles[gid].position.y * 5.0 + deltaTime * 1.5) * 0.1;
            particles[gid].position.x += turbulenceX * deltaTime;
            particles[gid].position.z += turbulenceZ * deltaTime;
    
            // Slow down as it rises
            particles[gid].velocity.y *= (1.0 - 0.1 * deltaTime);
        }
    }
    else
    {
        // Fade out based on remaining lifetime after emitter destruction
        float fadeOutSpeed = 1.0;  // Control fade speed
        particles[gid].color.a -= fadeOutSpeed * deltaTime;  // Linear fade-out
        particles[gid].color.a = max(particles[gid].color.a, 0.0);  // Clamp alpha to 0
    }
}
