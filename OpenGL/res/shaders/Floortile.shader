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

// Uniform for the time variable, to be set from the application
uniform float u_time;

void main()
{
    // Calculate the offset based on time
    float offset = u_time * 30.0; // Speed of movement, adjust as needed

    // Check if the block (2x2) is even or odd, with the offset
    if ((int((gl_FragCoord.x + offset) / 20) + int(gl_FragCoord.y / 20)) % 2 == 0)
    {
        color = vec4(1.0, 0.0, 0.0, 1.0); // Red
    }
    else
    {
        color = vec4(1.0, 0.0, 1.0, 1.0); // Pink
    }
}