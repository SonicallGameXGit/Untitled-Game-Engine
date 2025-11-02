#version 410

layout(location=0) in vec2 a_Position;
layout(location=0) out vec2 v_TexCoord;

uniform mat4 u_ProjectionViewMatrix;
uniform vec2 u_Position, u_Size;

void main() {
    gl_Position = u_ProjectionViewMatrix * vec4((a_Position * u_Size) + u_Position, 0.0, 1.0);
    v_TexCoord = a_Position;
}