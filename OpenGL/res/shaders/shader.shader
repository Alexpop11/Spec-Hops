#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

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
    v_TexCoord = texCoord;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 v_TexCoord;
uniform vec4 u_Color;
uniform sampler2D u_Texture;

#include "includes/lab.shader"

void main()
{
    vec4 texColor = texture(u_Texture, v_TexCoord);
    vec3 texColor_lab = rgb2lab(texColor.rgb);
    vec3 inputColor_lab = rgb2lab(u_Color.rgb);
    
    vec3 color_lab = mix(texColor_lab, inputColor_lab, u_Color.a);
    color = vec4(lab2rgb(color_lab), texColor.a);
}
