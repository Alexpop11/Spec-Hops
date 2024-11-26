struct Particle {
    position: vec2<f32>,  // World space position
    velocity: vec2<f32>,
    color: vec4<f32>,
};

@group(0) @binding(0) var<storage,read_write> particleBuffer: array<Particle>;


@compute @workgroup_size(32)
fn compute_main(@builtin(global_invocation_id) id: vec3<u32>) {
    // Apply the function f to the buffer element at index id.x:
    particleBuffer[id.x].position += particleBuffer[id.x].velocity * 0.01;
}
