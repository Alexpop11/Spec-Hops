#shader vertex
#version 330 core

layout(location = 0) in vec2 position;

uniform float u_AspectRatio;
uniform vec2 u_Position;

void main()
{
    vec4 adjustedPosition = vec4(position.xy, 0.0, 1.0);
    adjustedPosition.xy += u_Position;
    adjustedPosition.xy /= 18;
    adjustedPosition.xy -= 0.5;
    adjustedPosition.y *= u_AspectRatio;
    gl_Position = adjustedPosition;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
uniform vec4 u_Color;

#include "includes/lab.shader"

void main()
{
    color = vec4(1.0, 0.0, 0.0, 1.0);
}
