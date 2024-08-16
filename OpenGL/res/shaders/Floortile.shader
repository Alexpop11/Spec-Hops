#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec2 u_Resolution;
uniform vec4 u_BaseColor;  // Base color for the tile (sci-fi style)
uniform float u_Time;      // Animation over time for added effect

// Function to create a grid pattern
float gridPattern(vec2 uv, float gridSize) {
    vec2 grid = fract(uv * gridSize);
    return step(0.9, grid.x) * step(0.9, grid.y);
}

void main()
{
    vec2 uv = gl_FragCoord.xy / u_Resolution;
    
    // Scale down the UVs for a blocky/pixel effect
    float gridSize = 8.0; // The size of each "pixel" in the shader
    uv = floor(uv * gridSize) / gridSize;

    // Grid color based on position
    vec3 baseTileColor = vec3(0.2, 0.2, 0.2) + uv.yyx * 0.05; // Add a slight gradient for depth

    // Sci-fi floor panel lines
    float lines = gridPattern(uv * vec2(4.0, 4.0), 2.0);  // Creates grid lines across the tile
    vec3 lineColor = mix(baseTileColor, vec3(0.05, 0.05, 0.1), lines);

    // Add a metallic sheen effect using a cosine wave based on time
    float metallicEffect = abs(sin(u_Time + uv.y * 10.0)) * 0.1;
    lineColor += vec3(metallicEffect);

    // Apply some subtle "noise" for texture
    float noise = fract(sin(dot(uv.xy ,vec2(12.9898,78.233))) * 43758.5453);
    lineColor += vec3(noise * 0.05);

    color = vec4(lineColor, 1.0); // Final output color
}
