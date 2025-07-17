#version 410

layout(location=0) in vec4 a_Position;
layout(location=1) in vec2 a_TexCoord;

layout(location=0) out vec2 v_TexCoord;

void main() {
    gl_Position = a_Position;
    v_TexCoord = a_TexCoord;
}