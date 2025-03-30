#version 460 core

layout(local_size_x = 512) in;

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

// Shared memory for particles within a workgroup
shared Particle localParticles[512];

void main()
{
    uint gid = gl_GlobalInvocationID.x;
    uint lid = gl_LocalInvocationID.x;

    if (gid >= particles.length())
        return;

    // Load particle into shared memory
    localParticles[lid] = particles[gid];

    // Synchronize to ensure all particles are loaded
    barrier();

    if(emitterAlive)
    {
        // Update lifetime
        localParticles[lid].velocity.w -= deltaTime;

        //TODO: Too many heavy/redundant operations. 
        // If the particle is dead, respawn it
        if (localParticles[lid].velocity.w <= 0.0)
        {
            // Generate random values for spherical coordinates
            float u = fract(sin(float(gid) * 12.9898) * 43758.5453);
            float v = fract(sin(float(gid) * 78.233) * 43758.5453);

            // Spherical coordinates to cartesian
            float theta = u * 2.0 * 3.14159;
            float phi = acos(2.0 * v - 1.0);
            float r = sphereRadius * pow(fract(sin(float(gid) * 34.567) * 43758.5453), 1.0 / 3.0);

            localParticles[lid].position = vec4
            (
                particleEmitterCurrentPos.x + r * sin(phi) * cos(theta),
                particleEmitterCurrentPos.y + r * sin(phi) * sin(theta),
                particleEmitterCurrentPos.z + r * cos(phi),
                1.0 + fract(sin(float(gid) * 56.789) * 43758.5453) * 0.5
            );

            localParticles[lid].color = vec4
            (
                0.9, 0.9, 0.9,
                0.5 + 0.5 * fract(sin(float(gid) * 12.7) * 43758.5453)
            );

            localParticles[lid].velocity = vec4
            (
                mix(-0.2, 0.2, fract(sin(float(gid) * 32.8) * 43758.5453)),
                0.5 + 0.5 * fract(sin(float(gid) * 94.2) * 43758.5453),
                mix(-0.2, 0.2, fract(sin(float(gid) * 70.3) * 43758.5453)),
                maxLifetime * fract(sin(float(gid) * 18.6) * 43758.5453)
            );
        }
        else
        {
            // Update position based on velocity
            localParticles[lid].position.xyz += (localParticles[lid].velocity.xyz * deltaTime);

            // Handle base position movement
            vec3 baseDelta = particleEmitterCurrentPos - prevParticleEmitterPos;
            localParticles[lid].position.xyz += baseDelta;

            // Apply some wind effect
            localParticles[lid].velocity.x += 0.05 * deltaTime * sin(localParticles[lid].position.y * 0.5 + deltaTime * 0.2);

            // Age-based scaling
            float currentLifetime = localParticles[lid].velocity.w;
            float lifePercent = 1.0 - (currentLifetime / maxLifetime);

            float minSize = 0.8;
            float maxSize = 2.0;
            localParticles[lid].position.w = mix(minSize, maxSize, lifePercent);

            // Decrease opacity over time
            localParticles[lid].color.a = (1.0 - lifePercent) * 0.7;

            // Add some turbulence
            float turbulenceX = sin(localParticles[lid].position.y * 8.0 + deltaTime) * 0.3;
            float turbulenceZ = cos(localParticles[lid].position.y * 5.0 + deltaTime * 1.5) * 0.1;
            localParticles[lid].position.x += turbulenceX * deltaTime;
            localParticles[lid].position.z += turbulenceZ * deltaTime;

            // Slow down as it rises
            localParticles[lid].velocity.y *= (1.0 - 0.1 * deltaTime);
        }
    }
    else
    {
        // Fade out based on remaining lifetime after emitter destruction
        float fadeOutSpeed = 1.0;
        localParticles[lid].color.a -= fadeOutSpeed * deltaTime;
        localParticles[lid].color.a = max(localParticles[lid].color.a, 0.0);
    }

    // Synchronize before writing back to global memory
    barrier();

    // Write back updated particle to global memory
    particles[gid] = localParticles[lid];
}