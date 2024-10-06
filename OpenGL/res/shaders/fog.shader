#shader vertex
#version 330 core

layout(location = 0) in vec2 position;

uniform mat4 u_MVP;

out vec2 vWorldPosition; // Pass to fragment shader


void main()
{
    gl_Position = u_MVP * vec4(position, 0.0, 1.0);
    vWorldPosition = position;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 vWorldPosition; // Received from vertex shader

uniform vec4 u_Color;
uniform vec4 u_BandColor;
uniform vec2 uPlayerPosition; 

#include "includes/lab.shader"

void main()
{
    float distance = length(vWorldPosition - uPlayerPosition);

    float intensity = 1.0 / (1.0 + ((distance * distance) / 15));
    color = mix(u_Color, u_BandColor, intensity);
}
