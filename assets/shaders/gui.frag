#version 410

layout(location=0) in vec2 v_TexCoord;
layout(location=0) out vec4 f_Color;

uniform vec4 u_Color;
uniform sampler2D u_ColorSampler;
uniform bool u_HasTexture;
uniform vec2 u_ClipPosition;
uniform vec2 u_ClipSize;

void main() {
    if (gl_FragCoord.x < u_ClipPosition.x || gl_FragCoord.x > u_ClipPosition.x + u_ClipSize.x ||
        gl_FragCoord.y < u_ClipPosition.y || gl_FragCoord.y > u_ClipPosition.y + u_ClipSize.y) {
        discard;
    }
    f_Color = u_HasTexture ? texture2D(u_ColorSampler, v_TexCoord) * u_Color : u_Color;
}