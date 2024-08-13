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
uniform float u_Time; // Time variable to animate the stars
uniform vec2 u_Resolution; // Resolution of the screen

// Function to generate a pseudo-random value
float random(vec2 coord)
{
    return fract(sin(dot(coord, vec2(12.9898, 78.233))) * 43758.5453);
}

// Function to create a smooth star
float star(vec2 uv, vec2 center, float size)
{
    vec2 d = uv - center;
    float r = length(d);
    float brightness = 1.0 - smoothstep(0.0, size, r);
    return brightness * brightness; // Square for smoother falloff
}

void main()
{
    vec2 uv = gl_FragCoord.xy / u_Resolution.xy;
    
    // Number of large stars
    const int numStars = 100;
    
    vec3 finalColor = vec3(0.0); // Black background
    
    for (int i = 0; i < numStars; i++)
    {
        // Generate a unique seed for each star
        float seed = float(i) / float(numStars);
        
        // Generate random properties for each star
        float starSpeed = mix(0.05, 0.2, random(vec2(seed, 0.1)));
        float starSize = mix(0.005, 0.01, random(vec2(seed, 0.2)));
        float starY = random(vec2(seed, 0.3));
        
        // Calculate star position
        float starX = fract(seed + u_Time * starSpeed);
        vec2 starPos = vec2(starX, starY);
        
        // Add the star to the final color
        finalColor += vec3(star(uv, starPos, starSize));
    }
    
    // Add small background stars
    float smallStarBrightness = random(vec2(fract(uv.x + u_Time * 0.1), uv.y));
    if (smallStarBrightness > 0.995) // Adjust this value to control small star density
    {
        finalColor += vec3(0.3); // Dim white for small stars
    }
    
    color = vec4(finalColor, 1.0);
}