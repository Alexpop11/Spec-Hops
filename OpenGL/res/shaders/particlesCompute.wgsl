struct WorldInfo {
    deltaTime: f32,
    mousePos: vec2<f32>,
}

struct Particle {
    position: vec2<f32>,  // World space position
    velocity: vec2<f32>,
    color: vec4<f32>,
};

@group(0) @binding(0) var<storage,read_write> particleBuffer: array<Particle>;
@group(0) @binding(1) var<uniform> world: WorldInfo;

const G: f32 = 30.0; // Gravitational constant (adjusted for scale)
const MIN_DISTANCE_SQUARED: f32 = 1.0; // Prevent division by zero

@compute @workgroup_size(32)
fn compute_main(@builtin(global_invocation_id) id: vec3<u32>) {
    let particle = &particleBuffer[id.x];
    
    // Calculate direction to mouse
    let toMouse = world.mousePos - particle.position;
    let distanceSquared = max(dot(toMouse, toMouse), MIN_DISTANCE_SQUARED);
    
    // Calculate gravitational force (F = G * m1 * m2 / r^2)
    // Since mass is uniform we can simplify
    let force = normalize(toMouse) * G / distanceSquared;
    
    // Update velocity (a = F/m, simplified since mass = 1)
    particleBuffer[id.x].velocity += force * world.deltaTime;
    
    // Update position
    particleBuffer[id.x].position += particleBuffer[id.x].velocity * world.deltaTime;
}
