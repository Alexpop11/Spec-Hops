#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

uniform float u_AspectRatio;
uniform vec2 u_Position;


void main()
{
    vec4 adjustedPosition = position;
    adjustedPosition.xy += u_Position;
    adjustedPosition.xy /= 18;
    adjustedPosition.xy -= 0.5;
    adjustedPosition.y *= u_AspectRatio;
    gl_Position = adjustedPosition;
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

void main()
{
    vec2 uv = gl_FragCoord.xy / u_Resolution;
    
    // Offset the stars to create a drifting effect
    float starX = fract(uv.x + u_Time * 0.05);
    float starY = uv.y;
    
    // Generate a random star field
    float brightness = random(vec2(starX, starY));
    
    // Threshold to decide if a pixel is a star
    if (brightness > 0.995) // Adjust this value to control star density
    {
        color = vec4(1.0); // White star
    }
    else
    {
        color = vec4(0.0); // Black background
    }
}
