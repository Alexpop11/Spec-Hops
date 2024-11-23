struct Particle {
    @location(0) position: vec2<f32>,
    @location(1) velocity: vec2<f32>,
    @location(2) color: vec4<f32>,
};

struct VertexInput {
    @location(0) position: vec2<f32>,
};

struct VertexUniforms {
    u_MVP: mat4x4<f32>,
};

@group(0) @binding(0)
var<uniform> vertexUniforms: VertexUniforms;

struct VertexOutput {
    @builtin(position) Position: vec4<f32>,
    @location(0) color: vec4<f32>,
};

struct FragmentOutput {
    @location(0) color: vec4<f32>,
};

@vertex
fn vertex_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    output.Position = vertexUniforms.u_MVP * vec4<f32>(input.position, 0.0, 1.0);
    output.color = vec4<f32>(1.0, 0.0, 0.0, 1.0);
    return output;
}

@fragment
fn fragment_main(input: VertexOutput) -> FragmentOutput {
    var output: FragmentOutput;
    output.color = input.color;
    return output;
}
