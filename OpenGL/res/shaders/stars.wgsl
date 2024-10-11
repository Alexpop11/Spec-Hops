struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
}

@vertex 
fn vertex_main(@location(0) position: vec4<f32>) -> VertexOutput {
    return VertexOutput(position);
}

struct FragmentOutput {
    @location(0) color: vec4<f32>,
}

struct StarUniforms {
	time: f32,
	resolution: vec2f,
};

@group(0) @binding(0) var<uniform> uSTarUniforms: StarUniforms;
var<private> color: vec4<f32>;
var<private> gl_FragCoord_1: vec4<f32>;

fn random(coord: vec2<f32>) -> f32 {
    var coord_1: vec2<f32>;

    coord_1 = coord;
    let _e9 = coord_1;
    let _e18 = coord_1;
    let _e30 = coord_1;
    let _e39 = coord_1;
    return fract((sin(dot(_e39, vec2<f32>(12.9898f, 78.233f))) * 43758.547f));
}

fn star(uv: vec2<f32>, center: vec2<f32>, size: f32) -> f32 {
    var uv_1: vec2<f32>;
    var center_1: vec2<f32>;
    var size_1: f32;
    var d: vec2<f32>;
    var manhattanDist: f32;
    var euclideanDist: f32;
    var pointiness: f32 = 3f;
    var star_1: f32;
    var flareR: f32;
    var invertedflareR: f32;
    var flare: f32;

    uv_1 = uv;
    center_1 = center;
    size_1 = size;
    let _e9 = uv_1;
    let _e10 = center_1;
    let _e12 = uv_1;
    let _e13 = center_1;
    d = abs((_e12 - _e13));
    let _e17 = d;
    let _e19 = d;
    manhattanDist = (_e17.x + _e19.y);
    let _e24 = d;
    euclideanDist = length(_e24);
    let _e32 = size_1;
    let _e37 = size_1;
    let _e40 = euclideanDist;
    star_1 = (1f - smoothstep(0f, (_e37 / 1.5f), _e40));
    let _e46 = pointiness;
    let _e48 = manhattanDist;
    let _e49 = euclideanDist;
    let _e50 = pointiness;
    flareR = mix(_e48, _e49, -(_e50));
    let _e54 = size_1;
    let _e55 = flareR;
    invertedflareR = (_e54 - _e55);
    let _e58 = size_1;
    let _e63 = size_1;
    let _e68 = flareR;
    flare = smoothstep((_e63 + 0.01f), 0f, _e68);
    let _e73 = flare;
    let _e74 = star_1;
    return max(_e73, _e74);
}

fn starColor(seed: f32) -> vec3<f32> {
    var seed_1: f32;
    var colorType: f32;

    seed_1 = seed;
    let _e5 = seed_1;
    let _e8 = seed_1;
    let _e11 = random(vec2<f32>(_e8, 0.4f));
    colorType = _e11;
    let _e13 = colorType;
    if (_e13 < 0.79f) {
        {
            return vec3<f32>(1f, 1f, 1f);
        }
    } else {
        let _e20 = colorType;
        if (_e20 < 0.82f) {
            {
                return vec3<f32>(0.65f, 0.73f, 1f);
            }
        } else {
            let _e27 = colorType;
            if (_e27 < 0.9f) {
                {
                    return vec3<f32>(1f, 0.75f, 0.75f);
                }
            } else {
                {
                    return vec3<f32>(1f, 0.9f, 0.7f);
                }
            }
        }
    }
}

fn main_1() {
    var uv_2: vec2<f32>;
    var aspectRatio: f32;
    var numStars: i32 = 2i;
    var finalColor: vec3<f32> = vec3(0f);
    var i: i32 = 0i;
    var seed_2: f32;
    var speedF: f32;
    var local: f32;
    var local_1: f32;
    var starSpeed: f32;
    var starSize: f32;
    var starY: f32;
    var starX: f32;
    var starPos: vec2<f32>;
    var starTint: vec3<f32>;
    var smallStarBrightness: f32;

    let _e4 = gl_FragCoord_1;
    let _e6 = uSTarUniforms.resolution;
    uv_2 = (_e4.xy / _e6.xy);
    let _e10 = uSTarUniforms.resolution;
    let _e12 = uSTarUniforms.resolution;
    aspectRatio = (_e10.x / _e12.y);
    let _e17 = uv_2;
    let _e19 = aspectRatio;
    uv_2.x = (_e17.x * _e19);
    let _e22 = uv_2;
    let _e27 = uv_2;
    uv_2.x = (floor((_e27.x * 450f)) / 450f);
    let _e37 = uv_2;
    let _e42 = uv_2;
    uv_2.y = (floor((_e42.y * 450f)) / 450f);
    loop {
        let _e58 = i;
        let _e59 = numStars;
        if !((_e58 < _e59)) {
            break;
        }
        {
            let _e65 = i;
            let _e67 = numStars;
            seed_2 = (f32(_e65) / f32(_e67));
            let _e71 = seed_2;
            let _e74 = seed_2;
            let _e77 = random(vec2<f32>(_e74, 0.1f));
            speedF = _e77;
            let _e79 = speedF;
            if (_e79 < 0.995f) {
                let _e82 = speedF;
                if (_e82 < 0.975f) {
                    let _e90 = speedF;
                    local = mix(0.01f, 0.08f, _e90);
                } else {
                    local = 0.23f;
                }
                let _e94 = local;
                local_1 = _e94;
            } else {
                local_1 = 0.8f;
            }
            let _e97 = local_1;
            starSpeed = -(_e97);
            let _e102 = seed_2;
            let _e105 = seed_2;
            let _e108 = random(vec2<f32>(_e105, 0.2f));
            let _e111 = seed_2;
            let _e114 = seed_2;
            let _e117 = random(vec2<f32>(_e114, 0.2f));
            starSize = mix(0.001f, 0.005f, _e117);
            let _e120 = seed_2;
            let _e123 = seed_2;
            let _e126 = random(vec2<f32>(_e123, 0.3f));
            starY = _e126;
            let _e128 = seed_2;
            let _e129 = uSTarUniforms.time;
            let _e130 = starSpeed;
            let _e133 = seed_2;
            let _e134 = uSTarUniforms.time;
            let _e135 = starSpeed;
            starX = fract((_e133 + (_e134 * _e135)));
            let _e140 = starX;
            let _e141 = aspectRatio;
            let _e143 = starY;
            starPos = vec2<f32>((_e140 * _e141), _e143);
            let _e147 = seed_2;
            let _e148 = starColor(_e147);
            starTint = _e148;
            let _e150 = finalColor;
            let _e151 = starTint;
            let _e155 = uv_2;
            let _e156 = starPos;
            let _e157 = starSize;
            let _e158 = star(_e155, _e156, _e157);
            finalColor = (_e150 + (_e151 * _e158));
        }
        continuing {
            let _e62 = i;
            i = (_e62 + 1i);
        }
    }
    let _e162 = uv_2;
    let _e164 = aspectRatio;
    let _e167 = uv_2;
    let _e171 = uv_2;
    let _e173 = aspectRatio;
    let _e176 = uv_2;
    let _e179 = random(vec2<f32>((0.05f + (_e171.x / _e173)), _e176.y));
    smallStarBrightness = _e179;
    let _e181 = smallStarBrightness;
    if (_e181 > 0.985f) {
        {
            let _e184 = finalColor;
            finalColor = (_e184 + vec3(0.3f));
        }
    }
    let _e188 = finalColor;
    color = vec4<f32>(_e188.x, _e188.y, _e188.z, 1f);
    return;
}

@fragment 
fn fragment_main(@builtin(position) gl_FragCoord: vec4<f32>) -> FragmentOutput {
    gl_FragCoord_1 = gl_FragCoord;
    main_1();
    let _e9 = color;
    return FragmentOutput(_e9);
}
