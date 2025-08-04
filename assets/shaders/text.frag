#version 410

layout(location=0) in vec2 v_TexCoord;
layout(location=0) out vec4 f_Color;

uniform vec4 u_Color;
uniform float u_PixelRange;
uniform sampler2D u_ColorSampler;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange() {
    vec2 unitRange = vec2(u_PixelRange) / vec2(textureSize(u_ColorSampler, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(v_TexCoord);

    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main() {
    vec3 msd = texture2D(u_ColorSampler, v_TexCoord).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange() * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

    f_Color = vec4(1.0, 1.0, 1.0, opacity) * u_Color;
}