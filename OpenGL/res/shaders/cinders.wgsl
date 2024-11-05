[[block]]
struct Particle {
    position : vec2<f32>;
    velocity : vec2<f32>;
    color : vec4<f32>;
};

[[group(0), binding(0)]]
var<storage, read_write> particles : array<Particle>;

[[group(0), binding(1)]]
var<uniform> time : f32;

[[stage(compute), workgroup_size(64)]]
fn main([[builtin(global_invocation_id)]] id : vec3<u32>) {
    let index = id.x;
    if (index >= arrayLength(&particles)) {
        return;
    }

    var particle = particles[index];

    // Update position based on velocity
    particle.position += particle.velocity * time;

    // Fade color from red to gray
    let fadeAmount = 0.1 * time;
    particle.color = mix(particle.color, vec4<f32>(0.5, 0.5, 0.5, 1.0), fadeAmount);

    particles[index] = particle;
}
