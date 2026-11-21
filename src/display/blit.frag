#version 310 es
precision mediump float;
layout(binding = 0) uniform sampler2D u_fb;
layout(location = 0) out vec4 out_color;
void main() {
    out_color = texelFetch(u_fb, ivec2(gl_FragCoord.xy), 0);
}
