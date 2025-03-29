#version 460 core

in vec2 TexCoord;
in vec4 ParticleColor;

out vec4 FragColor;

uniform sampler2D smokeTexture;

void main() {
    // Sample the texture
    vec4 texColor = texture(smokeTexture, TexCoord);

    // Combine with particle color
    FragColor = texColor * ParticleColor;

    // Apply soft edges
    float distFromCenter = length(TexCoord - vec2(0.5));
    float fadeEdge = smoothstep(0.5, 0.4, distFromCenter);

    FragColor.a *= fadeEdge;
}