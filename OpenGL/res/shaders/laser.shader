#shader vertex
#version 330 core

// Vertex attributes
layout(location = 0) in vec2 a_Position;

// Uniforms
uniform vec2 u_StartPos;       // Start position of the line
uniform vec2 u_EndPos;         // End position of the line
uniform float u_Width;         // Width of the line

uniform mat4 u_MVP;            // Model-View-Projection matrix

// Output to Fragment Shader
out float v_WidthFactor;       // Factor to determine fragment's position relative to center

void main()
{
    // Calculate the direction vector from start to end
    vec2 direction = normalize(u_EndPos - u_StartPos);
    
    // Calculate the perpendicular vector for width offset
    vec2 perpendicular = vec2(-direction.y, direction.x);
    
    // Scale the perpendicular vector by width and vertex y position
    vec2 offset = perpendicular * a_Position.y * u_Width;
    
    // Interpolate between start and end positions based on vertex x position
    vec2 interpolatedPos = mix(u_StartPos, u_EndPos, a_Position.x);
    
    // Apply the offset to get the final position
    vec2 finalPos = interpolatedPos + offset;
    
    // Transform the final position using the MVP matrix
    gl_Position = u_MVP * vec4(finalPos, 0.0, 1.0);
    
    // Pass the absolute value of y position to fragment shader for gradient
    v_WidthFactor = abs(a_Position.y);
}


#shader fragment
#version 330 core

// Input from Vertex Shader
in float v_WidthFactor;

// Output color
layout(location = 0) out vec4 color;

// Uniform for line color
uniform vec4 u_Color;

void main()
{
    // Define the core and edge softness
    float core = 0.2; // Determines the width of the bright core
    float edge = 0.8; // Determines the width where the glow starts
    
    // Calculate alpha based on the width factor to create a smooth gradient
    float alpha = 1.0;
    
    // Smooth transition for the core
    alpha *= smoothstep(core, core + 0.1, 1.0 - v_WidthFactor);
    
    // Add a subtle glow towards the edges
    alpha += smoothstep(edge, edge + 0.2, 1.0 - v_WidthFactor) * 0.5;
    
    // Clamp the alpha to ensure it stays within [0,1]
    alpha = clamp(alpha, 0.0, 1.0);
    
    // Apply the calculated alpha to the color
    color = vec4(u_Color.rgb, alpha * u_Color.a);
}
