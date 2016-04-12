#version 330

in vec2 uv;

out vec3 color;

uniform sampler1D tex;

void main() {
    color = texture(tex, uv.x).rgb;
}
