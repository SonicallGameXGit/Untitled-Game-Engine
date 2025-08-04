#version 410

layout(location=0) in vec2 a_Position;
layout(location=1) in vec4 i_PositionTransform;
layout(location=2) in vec4 i_UvTransform;

layout(location=0) out vec2 v_TexCoord;

uniform mat4 u_ProjectionViewModelMatrix;

void main() {
    gl_Position = u_ProjectionViewModelMatrix * vec4(a_Position * i_PositionTransform.zw + i_PositionTransform.xy, 0.0, 1.0);
    v_TexCoord = a_Position * i_UvTransform.zw + i_UvTransform.xy;
}