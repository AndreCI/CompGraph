#version 330

in vec2 uv;

out vec3 color;
out float height;

uniform sampler2D tex;

void main() {
    color = vec3((fract(sin(dot(uv ,vec2(12.9898,78.233))) * 43758.5453)),(fract(sin(dot(uv ,vec2(12.9898,78.233))) * 43758.5453)),(fract(sin(dot(uv ,vec2(12.9898,78.233))) * 43758.5453)));
}
