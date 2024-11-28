// Define the WorldInfo struct
struct WorldInfo {
    deltaTime : f32,
    mousePos  : vec2<f32>,
};

// Define the Particle struct
struct Particle {
    position : vec2<f32>,  // World space position
    velocity : vec2<f32>,
    color    : vec4<f32>,
};

// Define the Segment struct (wall segments)
struct Segment {
    start : vec2<f32>,
    end   : vec2<f32>,
};

// Define the BVH Node struct
struct BvhNode {
    leftTypeCount : u32,    // leftType :1, leftCount :31
    leftOffset    : u32,

    rightTypeCount : u32,   // rightType :1, rightCount :31
    rightOffset    : u32,

    leftBBoxMin  : vec2<f32>,
    leftBBoxMax  : vec2<f32>,

    rightBBoxMin : vec2<f32>,
    rightBBoxMax : vec2<f32>,
};

// Define the AABB struct
struct AABB {
    min : vec2<f32>,
    max : vec2<f32>,
};

// Helper functions to unpack BVH Node data
fn getLeftType(node : BvhNode) -> u32 {
    return node.leftTypeCount >> 31u;
}

fn getLeftCount(node : BvhNode) -> u32 {
    return node.leftTypeCount & 0x7FFFFFFFu;
}

fn getRightType(node : BvhNode) -> u32 {
    return node.rightTypeCount >> 31u;
}

fn getRightCount(node : BvhNode) -> u32 {
    return node.rightTypeCount & 0x7FFFFFFFu;
}

fn isLeafNode(node : BvhNode, isLeft : bool) -> bool {
    if isLeft {
        return getLeftType(node) == 1u;
    } else {
        return getRightType(node) == 1u;
    }
}

fn getChildOffset(node : BvhNode, isLeft : bool) -> u32 {
    if isLeft {
        return node.leftOffset;
    } else {
        return node.rightOffset;
    }
}

fn getChildCount(node : BvhNode, isLeft : bool) -> u32 {
    if isLeft {
        return getLeftCount(node);
    } else {
        return getRightCount(node);
    }
}

fn getChildBBox(node : BvhNode, isLeft : bool) -> AABB {
    if isLeft {
        return AABB(node.leftBBoxMin, node.leftBBoxMax);
    } else {
        return AABB(node.rightBBoxMin, node.rightBBoxMax);
    }
}

// Utility functions
fn cross2D(a : vec2<f32>, b : vec2<f32>) -> f32 {
    return a.x * b.y - a.y * b.x;
}

fn dot2D(a : vec2<f32>, b : vec2<f32>) -> f32 {
    return a.x * b.x + a.y * b.y;
}

// Ray structure for collision detection
struct Ray {
    origin    : vec2<f32>,
    direction : vec2<f32>,
};

// Result of segment intersection
struct SegmentIntersection {
    hit      : bool,
    position : vec2<f32>,
    t        : f32,
};

// Buffer bindings
@group(0) @binding(0) var<storage, read_write> particleBuffer : array<Particle>;
@group(0) @binding(1) var<uniform> world : WorldInfo;

// Wall segments and BVH
@group(0) @binding(2) var<storage, read> bvhNodes : array<BvhNode>;
@group(0) @binding(3) var<storage, read> segments : array<Segment>;

// Uniforms to hold counts (since arrayLength() is not available for storage buffers in WGSL)
struct Counts {
    particleCount : u32,
    bvhNodeCount  : u32,
    segmentCount  : u32,
};

// @group(0) @binding(4) var<uniform> counts : Counts;


// Constants
const G : f32 = 30.0;                  // Gravitational constant (adjusted for scale)
const MIN_DISTANCE_SQUARED : f32 = 1.0; // Prevent division by zero
const PARTICLE_RADIUS : f32 = 0.5;     // Particle radius for collision detection

// Compute shader entry point
@compute @workgroup_size(256)
fn compute_main(@builtin(global_invocation_id) id : vec3<u32>) {
    let index = id.x;

    let particle = &particleBuffer[index];

    // Calculate direction to mouse
    let toMouse = world.mousePos - particle.position;
    let distanceSquared = max(dot2D(toMouse, toMouse), MIN_DISTANCE_SQUARED);

    // Calculate gravitational force (F = G * m1 * m2 / r^2)
    // Since mass is uniform we can simplify
    let force = 0.0;//normalize(toMouse) * G / distanceSquared;

    // Update velocity (a = F/m, simplified since mass = 1)
    particleBuffer[index].velocity += force * world.deltaTime;

    // Update position
    let newPosition = particle.position + particleBuffer[index].velocity * world.deltaTime;

    // Check for collision with walls
    // let collision = traverseBVH(particle.position, newPosition);
    let collision = collidesWithWalls(particle.position, newPosition);
    
    if (collision) {
        // Particle has collided with a wall
        particleBuffer[index].color = vec4<f32>(1.0, 0.0, 0.0, 1.0); // Set color to red
        particleBuffer[index].velocity = vec2<f32>(0.0, 0.0);
    }

    // Update particle position
    particleBuffer[index].position = newPosition;
}

fn collidesWithWalls(particlePosition : vec2<f32>, newPosition : vec2<f32>) -> bool {

    for (var i: u32 = 0; i < arrayLength(&segments); i++) {
        let segment = segments[i];
        if (segmentsIntersect(segment, Segment(particlePosition, newPosition))) {
            return true;
        }
    }
    return false;
}


// Function to check if two segments intersect
fn segmentsIntersect(s1 : Segment, s2 : Segment) -> bool {
    let p = s1.start;
    let r = s1.end - s1.start;
    let q = s2.start;
    let s = s2.end - s2.start;

    let r_cross_s = cross2D(r, s);
    let q_p = q - p;

    if (abs(r_cross_s) < 1e-8) {
        return false; // Lines are parallel
    }

    let t = cross2D(q_p, s) / r_cross_s;
    let u = cross2D(q_p, r) / r_cross_s;

    return (t >= 0.0 && t <= 1.0) && (u >= 0.0 && u <= 1.0);
}

