#version 330

uniform mat4 MVP;
in vec3 position;
out vec3 uv;
//mat4 myMatrix;

void main() {

    gl_Position = MVP * vec4(position, 1.0);
    uv = position;
}
