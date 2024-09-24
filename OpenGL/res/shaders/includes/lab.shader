// Constants for the conversion
const vec3 D65 = vec3(0.95047, 1.0, 1.08883);
const float epsilon = 0.008856;
const float kappa = 903.3;

// Helper function for the LAB conversion
float f(float t) {
    return t > epsilon ? pow(t, 1.0 / 3.0) : (kappa * t + 16.0) / 116.0;
}

// RGB to LAB conversion
vec3 rgb2lab(vec3 rgb) {
    // 1. Convert RGB to XYZ
    vec3 xyz = mat3(
        0.4124564, 0.3575761, 0.1804375,
        0.2126729, 0.7151522, 0.0721750,
        0.0193339, 0.1191920, 0.9503041
    ) * rgb;

    // 2. Scale by reference white
    xyz /= D65;

    // 3. Apply nonlinear transformation
    vec3 lab;
    lab.y = 116.0 * f(xyz.y) - 16.0;
    lab.x = 500.0 * (f(xyz.x) - f(xyz.y));
    lab.z = 200.0 * (f(xyz.y) - f(xyz.z));

    return lab;
}

// Inverse of the helper function
float f_inv(float t) {
    return t > epsilon ? t * t * t : (116.0 * t - 16.0) / kappa;
}

// LAB to RGB conversion
vec3 lab2rgb(vec3 lab) {
    // 1. Compute XYZ
    float fy = (lab.y + 16.0) / 116.0;
    float fx = lab.x / 500.0 + fy;
    float fz = fy - lab.z / 200.0;

    vec3 xyz;
    xyz.x = D65.x * f_inv(fx);
    xyz.y = D65.y * f_inv(fy);
    xyz.z = D65.z * f_inv(fz);

    // 2. XYZ to RGB
    vec3 rgb = mat3(
        3.2404542, -1.5371385, -0.4985314,
       -0.9692660,  1.8760108,  0.0415560,
        0.0556434, -0.2040259,  1.0572252
    ) * xyz;

    // Clamp to [0,1] range
    return clamp(rgb, 0.0, 1.0);
}
