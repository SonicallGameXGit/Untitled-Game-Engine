#version 410

layout(location=0) in vec2 v_TexCoord;
layout(location=0) out vec4 f_Color;

uniform vec4 u_Color;
uniform sampler2D u_ColorSampler;

void main() {
    f_Color = texture2D(u_ColorSampler, v_TexCoord) * u_Color;
}