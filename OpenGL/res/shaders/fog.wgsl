// Vertex Shader

// Define the structure for the MVP uniform
struct VertexUniforms {
    u_MVP: mat4x4<f32>
};

// Bind group 0, binding 0: MVP matrix
@group(0) @binding(0)
var<uniform> fogVertexUniforms: VertexUniforms;

// Define the input attributes for the vertex shader
struct VertexInput {
    @location(0) position: vec2<f32>
};

// Define the output structure from the vertex shader to the fragment shader
struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) vWorldPosition: vec2<f32>,
};

// Vertex shader entry point
@vertex
fn vertex_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    // Apply the MVP transformation
    output.position = fogVertexUniforms.u_MVP * vec4<f32>(input.position, 0.0, 1.0);
    // Pass the world position to the fragment shader
    output.vWorldPosition = input.position;
    return output;
}

// Fragment Shader

// Define the structure for fragment-related uniforms
struct FragmentUniforms {
    u_Color: vec4<f32>,
    u_BandColor: vec4<f32>,
    uPlayerPosition: vec2<f32>,
};

// Bind group 0, binding 1: Fragment uniforms
@group(0) @binding(1)
var<uniform> fogFragmentUniforms: FragmentUniforms;

// Define the input from the vertex shader
struct FragmentInput {
    @location(0) vWorldPosition: vec2<f32>,
};

// Define the output color for the fragment shader
struct FragmentOutput {
    @location(0) color: vec4<f32>,
};

// Fragment shader entry point
@fragment
fn fragment_main(input: FragmentInput) -> FragmentOutput {
    var output: FragmentOutput;
    
    // Calculate the distance between the world position and the player position
    let distance = length(input.vWorldPosition - fogFragmentUniforms.uPlayerPosition);
    
    // Compute intensity based on distance
    let intensity = 1.0 / (1.0 + ((distance * distance) / 15.0));
    
    // Mix the two colors based on intensity
    output.color = mix(fogFragmentUniforms.u_Color, fogFragmentUniforms.u_BandColor, intensity);
    
    return output;
}
