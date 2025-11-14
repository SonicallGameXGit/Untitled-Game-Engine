// Yo, guys, I'm planning to start using Slang for shaders, so the MSDF functions will be just imported in any other Slang shader files and I'll implement text rendering in 2D and 3D back!
#version 410

layout(location=0) in vec2 v_TexCoord;
layout(location=0) out vec4 f_Color;

uniform vec4 u_Color;
uniform float u_PixelRange;
uniform sampler2D u_ColorSampler;
uniform vec2 u_ClipPosition;
uniform vec2 u_ClipSize;
uniform float u_ViewportHeight;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange() {
    vec2 unitRange = vec2(u_PixelRange) / vec2(textureSize(u_ColorSampler, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(v_TexCoord);

    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main() {
    if (gl_FragCoord.x < u_ClipPosition.x || gl_FragCoord.x > u_ClipPosition.x + u_ClipSize.x ||
        u_ViewportHeight - gl_FragCoord.y < u_ClipPosition.y || u_ViewportHeight - gl_FragCoord.y > u_ClipPosition.y + u_ClipSize.y) {
        discard;
    }

    vec3 msd = texture2D(u_ColorSampler, v_TexCoord).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange() * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

    f_Color = vec4(1.0, 1.0, 1.0, opacity) * u_Color;
}