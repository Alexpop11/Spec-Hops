#shader vertex
#version 330 core
        
layout(location = 0) in vec4 position;
        
void main()
{
    gl_Position = position;
};
    
#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec2 u_Resolution;
uniform float u_Time;

void main() {
    vec2 uv = gl_FragCoord.xy / u_Resolution.xy;
    uv *= 10.0; // Scale up to create larger pixels
    
    vec2 grid = fract(uv);
    vec2 id = floor(uv);
    
    // Create alternating tile pattern
    float tilePattern = mod(id.x + id.y, 2.0);
    
    // Base colors
    vec3 darkTile = vec3(0.1, 0.2, 0.3);
    vec3 lightTile = vec3(0.2, 0.3, 0.4);
    
    // Choose tile color based on pattern
    vec3 tileColor = mix(darkTile, lightTile, tilePattern);
    
    // Add grid lines
    float gridLine = step(0.9, grid.x) + step(0.9, grid.y);
    vec3 gridColor = vec3(0.0, 0.7, 1.0);
    
    // Add a "glowing" effect to some tiles
    float glow = sin(id.x * 0.2 + id.y * 0.3 + u_Time) * 0.5 + 0.5;
    vec3 glowColor = vec3(0.0, 0.9, 1.0) * glow;
    
    // Combine everything
    vec3 finalColor = mix(tileColor, gridColor, gridLine);
    finalColor += glowColor * 0.3;
    
    color = vec4(finalColor, 1.0);
}