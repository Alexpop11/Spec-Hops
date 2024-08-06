#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

uniform float u_AspectRatio;

void main()
{
    vec4 adjustedPosition = position;
    adjustedPosition.y *= u_AspectRatio;
    gl_Position = adjustedPosition;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
uniform vec4 u_Color;

void main()
{
    color = u_Color;
}
