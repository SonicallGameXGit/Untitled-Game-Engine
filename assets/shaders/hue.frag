#version 410

layout(location=0) in vec2 v_TexCoord;
layout(location=0) out vec4 f_Color;

uniform float u_Time;
uniform sampler2D u_ColorSampler;

// Source: https://gist.github.com/fschr/a8476f8993e5b9a60aa1c7ec4af3704b
vec3 hueShift(vec3 color, float hueAdjust) {
    const vec3 kRGBToYPrime = vec3(0.299, 0.587, 0.114);
    const vec3 kRGBToI = vec3(0.596, -0.275, -0.321);
    const vec3 kRGBToQ = vec3(0.212, -0.523, 0.311);

    const vec3 kYIQToR = vec3(1.0, 0.956, 0.621);
    const vec3 kYIQToG = vec3(1.0, -0.272, -0.647);
    const vec3 kYIQToB = vec3(1.0, -1.107, 1.704);

    float YPrime = dot(color, kRGBToYPrime);
    float I = dot(color, kRGBToI);
    float Q = dot(color, kRGBToQ);
    float hue = atan(Q, I);
    float chroma = sqrt(I * I + Q * Q);

    hue += hueAdjust;

    Q = chroma * sin (hue);
    I = chroma * cos (hue);

    vec3 yIQ = vec3 (YPrime, I, Q);

    return vec3(dot(yIQ, kYIQToR), dot(yIQ, kYIQToG), dot(yIQ, kYIQToB));
}

void main() {
    f_Color = texture2D(u_ColorSampler, v_TexCoord);
    f_Color.rgb = hueShift(f_Color.rgb, u_Time);
}