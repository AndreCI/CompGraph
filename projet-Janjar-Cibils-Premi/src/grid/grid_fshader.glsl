#version 330

in vec2 uv;

out vec3 color;
in float height;
in float isRiver;

uniform sampler2D colorTex;
uniform sampler2D tex;
uniform sampler2D texture_snow;
uniform sampler2D texture_rock;
uniform sampler2D texture_grass;
uniform sampler2D texture_sand;

const vec3 COLORS[3] = vec3[](
  vec3(1.0,0,0),
vec3(0,1.0,0),
vec3(0,0,1.0)
);


vec3 getColorFromTexture(){
    vec3 pos = (vec3(uv.x,uv.y,height));
    vec3 Y = normalize(dFdy(pos));
    vec3 X = normalize(dFdx(pos));
    vec3 normal = (cross(X,Y));
    //normal = normal a un point
    vec3 light_dir = normalize(vec3(1,1,5));
    //light_dir = direction de la lumiere
    vec3 kd = vec3(1,1,1);//texture(tex,uv).rgb;
    float borne_j_v = 0.2;
    float borne_v = 0.5;
    float borne_v_b = 0.6;
    float borne_b = 0.8;
    vec3 couleurTop = texture(texture_snow,uv).rgb;
    vec3 couleurMid = texture(texture_rock,uv).rgb;
    vec3 couleurBot = texture(texture_sand,uv).rgb;
    if(height>borne_b){
        kd = couleurTop;
    }else if(height>borne_v_b){
        kd = vec3(couleurTop.x-(couleurTop.x-couleurMid.x)*(borne_b-height)/(borne_b-borne_v_b),
                  couleurTop.y-(couleurTop.y-couleurMid.y)*(borne_b-height)/(borne_b-borne_v_b),
                  couleurTop.z-(couleurTop.z-couleurMid.z)*(borne_b-height)/(borne_b-borne_v_b));
    }else if(height>borne_v){
        kd = couleurMid;
    }else if(height>borne_j_v){
        kd = vec3(couleurMid.x-(couleurMid.x-couleurBot.x)*(borne_v-height)/(borne_v-borne_j_v),
                  couleurMid.y-(couleurMid.y-couleurBot.y)*(borne_v-height)/(borne_v-borne_j_v),
                  couleurMid.z-(couleurMid.z-couleurBot.z)*(borne_v-height)/(borne_v-borne_j_v));
    }else{
        kd = couleurBot;
    }
    //kd = kd/255;
   // kd = vec3(height,height,1-height);
       //kd=vec3(0.9,0.9,0.9);
    //kd = couleur du matériel
    vec3 Ld = normalize(vec3(1,1,1));
    //Ld = couleur du soleil
    return kd;//* dot(normal,light_dir)*Ld;
}

void main() {
    color = vec3(0.0,0.0,0.0); //DO NOT TOUCH OR IT WILL BLOW UP
    vec3 texture_color = getColorFromTexture();
    if(isRiver==1){
        texture_color = vec3(0,0,1);
    }
    color = texture_color;
}
