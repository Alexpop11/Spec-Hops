// Vertex Uniforms
struct VertexUniforms {
    u_MVP: mat4x4<f32>;
};

// Fragment Uniforms
struct FragmentUniforms {
    u_Color: vec4<f32>;
};

// Vertex Input Structure
struct VertexInput {
    @location(0) position: vec2<f32>,
    @location(1) texCoord: vec2<f32>,
};

// Vertex Output Structure
struct VertexOutput {
    @builtin(position) Position: vec4<f32>,
    @location(0) v_TexCoord: vec2<f32>,
};

// Fragment Output Structure
struct FragmentOutput {
    @location(0) color: vec4<f32>,
};

// Bind Group 0 Bindings
@group(0) @binding(0)
var<uniform> vertexUniforms: VertexUniforms;

@group(0) @binding(1)
var<uniform> fragmentUniforms: FragmentUniforms;

@group(0) @binding(2)
var u_Texture: texture_2d<f32>;

@group(0) @binding(3)
var u_Sampler: sampler;

// Vertex Shader Entry Point
@vertex
fn vertex_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    // Transform the vertex position
    output.Position = vertexUniforms.u_MVP * vec4<f32>(input.position, 0.0, 1.0);
    // Pass through the texture coordinate
    output.v_TexCoord = input.texCoord;
    return output;
}

// Fragment Shader Entry Point
@fragment
fn fragment_main(input: VertexOutput) -> FragmentOutput {
    var output: FragmentOutput;
    // Sample the texture color
    let texColor = textureSample(u_Texture, u_Sampler, input.v_TexCoord);
    // Mix the texture color with the uniform color based on the alpha value of u_Color
    let mixedColor = mix(texColor, fragmentUniforms.u_Color, fragmentUniforms.u_Color.a);
    // Set the final color, preserving the alpha from the texture
    output.color = vec4<f32>(mixedColor.rgb, texColor.a);
    return output;
}
