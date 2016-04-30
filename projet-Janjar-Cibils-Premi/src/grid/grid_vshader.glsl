#version 330

in vec2 position;

out vec2 uv;
out float height;

uniform sampler2D heightTex;
uniform mat4 MVP;
uniform float time;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    /* TODO 1.2 : 2 Create a shader program for terrain rendering.
The vertex shader samples the height map texture an displaces the vertices according to the height.
(reuse/adapt from HW3)*/

   height = ((texture(heightTex,uv).x + texture(heightTex,uv).y)/2);

  // height =(texture(colorTex,uv).x + texture(colorTex,uv).y);

    vec3 pos_3d = vec3(position.x, height, -position.y);
    gl_Position = MVP * vec4(pos_3d, 1.0);
}
