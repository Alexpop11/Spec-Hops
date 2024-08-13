#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
uniform float u_AspectRatio;
uniform vec2 u_Position;
void main()
{
    gl_Position = position;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;
uniform float u_Time;
uniform vec2 u_Resolution;

float random(vec2 coord)
{
    return fract(sin(dot(coord, vec2(12.9898, 78.233))) * 43758.5453);
}

float star(vec2 uv, vec2 center, float size)
{
    vec2 d = abs(uv - center);
    float manhattanDist = d.x + d.y;
    float euclideanDist = length(d);
    float pointiness = 3;

    float star = 1.0 - smoothstep(0.0, size / .25, euclideanDist);
    
    // Combine Manhattan and Euclidean distances
    float flareR = mix(manhattanDist, euclideanDist, -pointiness);
    float invertedflareR = size - flareR;
    float flare = smoothstep(size+0.01, 0, flareR);

    
    // Combine the star and the aura
    return max(flare, star);
}

vec3 starColor(float seed)
{
    float colorType = random(vec2(seed, 0.4));
    
    if (colorType < 0.33) {
        // Pale blue
        return vec3(0.8, 0.9, 1.0);
    } else if (colorType < 0.66) {
        // Pale yellow
        return vec3(1.0, 1.0, 0.8);
    } else {
        // Pale red
        return vec3(1.0, 0.8, 0.8);
    }
}

void main()
{
    vec2 uv = gl_FragCoord.xy / u_Resolution.xy;
    float aspectRatio = u_Resolution.x / u_Resolution.y;
    
    // Adjust UV coordinates to maintain circular shape
    uv.x *= aspectRatio;
    
    const int numStars = 600;
    
    vec3 finalColor = vec3(0.0);
    
    for (int i = 0; i < numStars; i++)
    {
        float seed = float(i) / float(numStars);
        
        float starSpeed = mix(0.05, 0.15, random(vec2(seed, 0.1)));
        float starSize = mix(0.0008, 0.002, random(vec2(seed, 0.2)));
        float starY = random(vec2(seed, 0.3));
        
        float starX = fract(seed + u_Time * starSpeed);
        vec2 starPos = vec2(starX * aspectRatio, starY);
        
        vec3 starTint = starColor(seed);
        finalColor += starTint * star(uv, starPos, starSize);
    }
    
    float smallStarBrightness = random(vec2(uv.x / aspectRatio, uv.y) + u_Time * 0.1);
    if (smallStarBrightness > 0.995)
    {
        finalColor += vec3(0.3);
    }
    
    color = vec4(finalColor, 1.0);
}