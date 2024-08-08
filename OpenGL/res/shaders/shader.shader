#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

uniform float u_AspectRatio;
uniform vec2 u_Position;


void main()
{
    vec4 adjustedPosition = position;
    adjustedPosition.xy /= 18;
    adjustedPosition.xy += u_Position;
    adjustedPosition.xy -= 0.5;
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
