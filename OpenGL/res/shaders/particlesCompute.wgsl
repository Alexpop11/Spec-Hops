struct WorldInfo {
    deltaTime : f32,
};

struct Particle {
    position : vec2<f32>,  // World space position
    velocity : vec2<f32>,
    color    : vec4<f32>,
    age      : f32,
    lifetime : f32,
};

struct Segment {
    start : vec2<f32>,
    end   : vec2<f32>,
};

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

struct Ray {
    origin    : vec2<f32>,
    direction : vec2<f32>,
};

struct SegmentIntersection {
    hit      : bool,
    position : vec2<f32>,
    normal   : vec2<f32>,
    t        : f32,
};

// Buffer bindings
@group(0) @binding(0) var<storage, read_write> particleBuffer : array<Particle>;
@group(0) @binding(1) var<uniform> world : WorldInfo;
@group(0) @binding(2) var<storage, read> segments : array<Segment>;
@group(0) @binding(3) var<storage, read> bvhNodes : array<BvhNode>;

// Constants
const G : f32 = 30.0; 
const MIN_DISTANCE_SQUARED : f32 = 1.0; // Prevent division by zero

// Compute shader entry point
@compute @workgroup_size(256)
fn compute_main(@builtin(global_invocation_id) id : vec3<u32>) {
    let index = id.x;

    let particle = &particleBuffer[index];

    // New position based on current velocity
    let newPosition = particle.position + particleBuffer[index].velocity * world.deltaTime;

    // Check for collision with walls
    let intersection = findWallCollision(particle.position, newPosition);
    
    if (intersection.hit) {
        // Bounce coefficient (1.0 = perfect bounce, 0.0 = full stop)
        let bounce = 0.8;
        
        // Calculate reflection vector
        let v = particleBuffer[index].velocity;
        let n = intersection.normal;
        let reflected = v - 2.0 * dot2D(v, n) * n;
        
        // Update velocity with bounce effect
        let newVelocity = reflected * bounce;
        particleBuffer[index].velocity = newVelocity;
        
        // Place particle at intersection point
        particleBuffer[index].position = intersection.position + newVelocity * world.deltaTime;
    } else {
        // No collision, update particle position normally
        particleBuffer[index].position = newPosition;
    }
    
    // Update age
    particleBuffer[index].age += world.deltaTime;
}

fn findWallCollision(particlePosition : vec2<f32>, newPosition : vec2<f32>) -> SegmentIntersection {
    var closest : SegmentIntersection;
    closest.hit = false;
    closest.t = 999999.0;

    let particlePath = Segment(particlePosition, newPosition);
    
    for (var i: u32 = 0; i < arrayLength(&segments); i++) {
        let wall = segments[i];
        let intersection = segmentIntersection(wall, particlePath);
        
        if (intersection.hit && intersection.t < closest.t) {
            closest = intersection;
        }
    }
    
    return closest;
}

fn segmentIntersection(s1 : Segment, s2 : Segment) -> SegmentIntersection {
    var result : SegmentIntersection;
    result.hit = false;
    
    let p = s1.start;
    let r = s1.end - s1.start;
    let q = s2.start;
    let s = s2.end - s2.start;

    let r_cross_s = cross2D(r, s);
    let q_p = q - p;

    if (abs(r_cross_s) < 1e-8) {
        return result; // Lines are parallel
    }

    let t = cross2D(q_p, s) / r_cross_s;
    let u = cross2D(q_p, r) / r_cross_s;

    if (t >= 0.0 && t <= 1.0 && u >= 0.0 && u <= 1.0) {
        result.hit = true;
        result.t = t;
        result.position = p + t * r;
        result.normal = normalize(vec2<f32>(-r.y, r.x)); // Perpendicular to wall
        return result;
    }

    return result;
}

