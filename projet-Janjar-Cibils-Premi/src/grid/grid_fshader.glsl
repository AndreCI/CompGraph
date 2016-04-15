#version 330

in vec2 uv;

out vec3 color;
in float height;

uniform sampler2D colorTex;
uniform sampler2D tex;

void main() {
    color = vec3(0.0,0.0,0.0); //DO NOT TOUCH OR IT WILL BLOW UP

    /*TODO 1.2 : 3 Diffuse shading of the terrain in the fragment shader.
You have calculate the normal at every position in the fragment shader.
One way to do this is by using finite differences: you can find the gradient
at the current pixel position bycomparing the elevation with the elevation of
the neighboring pixels in the height map*/
    vec3 pos = (vec3(uv.x,uv.y,height));
    vec3 Y = normalize(dFdy(pos));
    vec3 X = normalize(dFdx(pos));
    vec3 normal = (cross(X,Y));
    //normal = normal a un point
    vec3 light_dir = normalize(vec3(10,10,10));
    //light_dir = direction de la lumiere
    vec3 kd = vec3(1,1,1);//texture(tex,uv).rgb;
   if(height>0.8){
        kd = vec3(255-height*(255-229),255,153+height*(204-153));
    }else if(height>0.3){
        kd=vec3(255-(height-0.3)/0.5*(255-204),255,153);
    }else{
        kd=vec3(255,255,153);
    }
    kd = normalize(kd);
   // kd = vec3(height,height,1-height);

    //kd = couleur du matériel
    vec3 Ld = normalize(vec3(1,1,1));
    //Ld = couleur du soleil

    color = kd * dot(normal,light_dir) * Ld;
}
