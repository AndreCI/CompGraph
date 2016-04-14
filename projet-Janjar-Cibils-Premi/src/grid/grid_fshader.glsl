#version 330

in vec2 uv;
out vec3 color;
out float height;
uniform sampler2D tex;


in vec3 light_dir;
uniform vec3 kd,Ld;



void main() {

    /*TODO 1.2 : 3 Diffuse shading of the terrain in the fragment shader.
You have to calculate the normal at every position in the fragment shader.
One way to do this is by using finite differences: you can find the gradient at the current pixel position by
comparing the elevation with the elevation of the neighboring pixels in the height map*/


    vec3 X = dFdx(vec3(uv.x,uv.y,height));
    vec3 Y = dFdy(vec3(uv.x,uv.y,height));
    vec3 normal = (cross(X,Y));
    vec3 n = normalize(normal);

    vec3 l = normalize(light_dir);
    vec3 Id = kd * dot(n,l) * Ld;
    //kd = couleur du matériel
    //Ld = couleur du soleil
    //light_dir = a nous de choisir ou est le soleil
   // color = texture(tex, uv).rgb;
    color = Id;
}
