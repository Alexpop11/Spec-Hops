// Vertex Uniforms
struct VertexUniforms {
    u_StartPos: vec2<f32>,
    u_EndPos: vec2<f32>,
    u_Width: f32,
    u_MVP: mat4x4<f32>,
};

@group(0) @binding(0)
var<uniform> vertexUniforms: VertexUniforms;

// Vertex Input Structure
struct VertexInput {
    @location(0) a_Position: vec2<f32>,
};

// Vertex Output Structure
struct VertexOutput {
    @builtin(position) Position: vec4<f32>,
};

// Vertex Shader Entry Point
@vertex
fn vertex_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;

    // Calculate the direction vector from start to end
    let direction = normalize(vertexUniforms.u_EndPos - vertexUniforms.u_StartPos);

    // Calculate the perpendicular vector for width offset
    let perpendicular = vec2<f32>(-direction.y, direction.x);

    // Scale the perpendicular vector by width and vertex y position
    let offset = perpendicular * input.a_Position.y * vertexUniforms.u_Width;

    // Interpolate between start and end positions based on vertex x position
    let interpolatedPos = mix(vertexUniforms.u_StartPos, vertexUniforms.u_EndPos, input.a_Position.x);

    // Apply the offset to get the final position
    let finalPos = interpolatedPos + offset;

    // Transform the final position using the MVP matrix
    output.Position = vertexUniforms.u_MVP * vec4<f32>(finalPos, 0.0, 1.0);

    return output;
}

// Fragment Uniforms
struct FragmentUniforms {
    u_Color: vec4<f32>,
};

@group(0) @binding(1)
var<uniform> fragmentUniforms: FragmentUniforms;

// Fragment Output Structure
struct FragmentOutput {
    @location(0) color: vec4<f32>,
};

// Fragment Shader Entry Point
@fragment
fn fragment_main() -> FragmentOutput {
    var output: FragmentOutput;
    output.color = fragmentUniforms.u_Color;
    return output;
}
