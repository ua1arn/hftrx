#version 310 es
precision mediump float;
layout(std140, binding = 0) uniform UBO { vec4 u_color; };
layout(location = 0) out vec4 out_color;
void main() {
    out_color = u_color;
}
