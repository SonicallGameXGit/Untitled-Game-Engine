#version 410

layout(location=0) in vec2 a_Position;
layout(location=0) out vec2 v_TexCoord;

uniform mat4 u_ProjectionViewMatrix, u_ModelMatrix;

void main() {
    gl_Position = u_ProjectionViewMatrix * u_ModelMatrix * vec4(a_Position, 0.0, 1.0);
    v_TexCoord = a_Position;
}