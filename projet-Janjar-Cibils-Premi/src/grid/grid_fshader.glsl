#version 330

in vec2 uv;

out vec3 color;
out float height;

uniform sampler2D tex;

void main() {
    color = vec3(0.0,0.0,0.0); //DO NOT TOUCH OR IT WILL BLOW UP

    /*TODO 1.2 : 3 Diffuse shading of the terrain in the fragment shader.
You have calculate the normal at every position in the fragment shader.
One way to do this is by using finite differences: you can find the gradient
at the current pixel position bycomparing the elevation with the elevation of
the neighboring pixels in the height map*/
    //kd = couleur du matériel
    //Ld = couleur du soleil
    vec3 Y = dFdy(vec3(uv,height));
    vec3 X = dFdx(vec3(uv,height));
    vec3 normal = normalize(cross(X,Y));

    vec3 kd = normalize(vec3(0.8,0.8,0.8));
    vec3 Ld = normalize(vec3(0.8,0.8,0.8));

    color = kd*normal*Ld;
}
