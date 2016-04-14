#version 330

in vec2 uv;

out vec3 color;
out float height;
uniform sampler2D tex;

void main() {
    color = texture(tex, uv).rgb;
}
