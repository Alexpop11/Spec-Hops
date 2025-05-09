struct Particle {
    @location(1) position: vec2<f32>,  // World space position
    @location(2) velocity: vec2<f32>,
    @location(3) color: vec4<f32>,
    @location(4) age: f32,
    @location(5) lifetime: f32,
};

struct VertexInput {
    @location(0) position: vec2<f32>,  // Local vertex position (relative to particle center)
};

// Separate matrices for clearer transform chain
struct VertexUniforms {
    world_to_clip: mat4x4<f32>,    // View-Projection matrix only
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
fn vertex_main(vertex: VertexInput, particle: Particle) -> VertexOutput {
    var output: VertexOutput;
    
    // Transform the local vertex position to world space relative to particle position
    let world_pos = vertex.position + particle.position;
    
    // Transform from world space to clip space using world_to_clip matrix
    output.Position = vertexUniforms.world_to_clip * vec4<f32>(world_pos, 0.0, 1.0);
    // Calculate opacity based on remaining lifetime
    var finalColor = particle.color;
    finalColor.a *= 1.0 - clamp(particle.age / particle.lifetime, 0.0, 1.0);
    output.color = finalColor;
    return output;
}

@fragment
fn fragment_main(input: VertexOutput) -> FragmentOutput {
    var output: FragmentOutput;
    output.color = input.color;
    return output;
}
