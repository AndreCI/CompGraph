#version 330

in vec3 TexCoord;
uniform mat4 MVP;

in vec3 position;

out vec3 uv;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    uv = TexCoord;
}
