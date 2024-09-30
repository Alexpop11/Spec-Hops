#shader vertex
#version 330 core

layout(location = 0) in vec2 position;

uniform mat4 u_MVP;

void main()
{
    gl_Position = u_MVP * vec4(position, 0.0, 1.0);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
uniform vec4 u_Color;

#include "includes/lab.shader"

void main()
{
    color = vec4(0.1, 0.1, 0.1, 1.0);
}
