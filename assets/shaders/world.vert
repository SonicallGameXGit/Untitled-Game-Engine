#version 410

layout(location=0) in vec3 a_Position;
layout(location=1) in vec2 a_TexCoord;
layout(location=2) in vec3 a_Normal;

layout(location=0) out vec3 v_Position;
layout(location=1) out vec2 v_TexCoord;
layout(location=2) out vec3 v_Normal;

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_ModelMatrix;

void main() {
    vec4 worldPosition = u_ModelMatrix * vec4(a_Position, 1.0);
    gl_Position = u_ProjectionViewMatrix * worldPosition;

    v_TexCoord = a_TexCoord;
    v_Normal = (u_ModelMatrix * vec4(a_Normal, 0.0)).xyz;
    v_Position = worldPosition.xyz;
}