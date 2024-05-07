#version 460

layout(location = 0) in vec2 f_uv;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D image;

void main() {
    vec2 uv = vec2(f_uv.x, -f_uv.y);
    outColor = vec4(pow(texture(image, uv).rgb, vec3(1.0 / 2.2)), 1.0);
}