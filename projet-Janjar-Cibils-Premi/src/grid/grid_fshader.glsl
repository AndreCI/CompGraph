#version 330

in vec2 uv;

layout (location = 0) out vec3 color;
layout (location = 1) out float currentHeight;
in float height;
in float isWater;

uniform sampler2D colorTex;
uniform sampler2D tex;
uniform sampler2D texture_snow;
uniform sampler2D texture_rock;
uniform sampler2D texture_grass;
uniform sampler2D texture_sand;
uniform sampler2D texture_water;

const vec3 COLORS[3] = vec3[](
  vec3(1.0,0,0),
vec3(0,1.0,0),
vec3(0,0,1.0)
);


vec3 get_kd_fBm(){
    vec3 kd = vec3(1,1,1);
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

    return kd;
}

vec3 getColorFrom_kd(vec3 kd){
    //kd = couleur du matériel
    vec3 pos = (vec3(uv.x,uv.y,height));
    vec3 Y = normalize(dFdy(pos));
    vec3 X = normalize(dFdx(pos));
    vec3 normal = (cross(X,Y));
    //normal = normale a un point(uv)
    vec3 light_dir = normalize(vec3(0,0,5));
    //light_dir = direction de la lumiere
    vec3 Ld = (vec3(1,1,1));
    //Ld = couleur du soleil
    //clamp dot product?
    return kd* dot(normal,light_dir) * Ld*2;
}

vec3 get_kd_water(vec3 texture_to_mix){
    if(isWater==1){
        vec3 grass_ = texture(texture_grass,uv).rgb;
        return mix(grass_,(texture(texture_water,uv)).rgb,0.9);//.rgb,texture_to_mix,0.5);
    }else{
        vec3 grass_ = texture(texture_grass,uv).rgb;
        return mix(grass_,texture_to_mix,0.8);
         return grass_;
    }
}

void main() {
    color = vec3(0.0,0.0,0.0); //DO NOT TOUCH OR IT WILL BLOW UP
    vec3 kd_fBm = get_kd_fBm();
    if(isWater!=0){
        kd_fBm = get_kd_water(kd_fBm);
    }
    color = getColorFrom_kd(kd_fBm);
    currentHeight = height;
}
