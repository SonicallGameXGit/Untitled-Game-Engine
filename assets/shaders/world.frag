#version 410

layout(location=0) in vec2 v_TexCoord;
layout(location=1) in vec3 v_Normal;

layout(location=0) out vec4 f_Color;

uniform vec4 u_Color;
uniform sampler2D u_ColorSampler;
uniform bool u_HasTexture;

float diffuse_ApplyAmbient(float diffuse, float factor) {
    return diffuse * (1.0 - factor) + factor;
}

void main() {
    vec4 albedo = u_HasTexture ? texture(u_ColorSampler, v_TexCoord) : vec4(1.0);
    albedo *= u_Color;

    const vec3 lightDirection = normalize(vec3(0.3, 1.0, 0.6));
    float diffuse = dot(normalize(v_Normal), lightDirection);
    diffuse = max(diffuse, 0.0);
    diffuse = diffuse_ApplyAmbient(diffuse, 0.3);

    f_Color = albedo * diffuse;
}