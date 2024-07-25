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

uniform float u_time;
uniform vec2 u_resolution;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i + vec2(0.0, 0.0)), 
                   hash(i + vec2(1.0, 0.0)), u.x),
               mix(hash(i + vec2(0.0, 1.0)), 
                   hash(i + vec2(1.0, 1.0)), u.x), u.y);
}

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution;
    uv *= 10.0;

    // Add time-based movement
    uv.x += u_time * 0.5;

    // Generate noise-based texture
    float n = noise(uv);

    // Create the tile pattern
    float tile = step(0.5, fract(uv.x)) * step(0.5, fract(uv.y));

    // Combine noise and tile pattern for the floor texture
    vec3 floorColor = mix(vec3(0.2, 0.2, 0.2), vec3(0.7, 0.7, 0.7), tile * n);

    // Add metallic highlights
    float highlight = pow(1.0 - abs(2.0 * fract(uv.x) - 1.0), 5.0) + 
                      pow(1.0 - abs(2.0 * fract(uv.y) - 1.0), 5.0);
    floorColor += highlight * 0.1;

    color = vec4(floorColor, 1.0);
}