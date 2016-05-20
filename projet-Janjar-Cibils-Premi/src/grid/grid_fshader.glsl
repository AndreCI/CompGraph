#version 330

in vec2 uv;

out vec3 color;
in float height;
in float isWater;

uniform sampler2D colorTex;
uniform sampler2D mirrorTex;
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

float random(vec2 p){return fract(cos(dot(p,vec2(23.14069263277926,2.665144142690225)))*123456.);}

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
    vec3 light_dir = normalize(vec3(0,10,0));
    vec3 Ld = (vec3(1,1,1));
    return kd * dot(normal,light_dir) * Ld;
}

vec3 get_kd_water(vec3 texture_to_mix){
    if(isWater==1 || height<0.22){
        float window_width = textureSize(mirrorTex,0).x;
        float window_height = textureSize(mirrorTex,0).y;
        float _u =gl_FragCoord.x/window_width;
        float _v = 1-gl_FragCoord.y/window_height;
       return mix(texture(mirrorTex,vec2(_u,_v)).rgb,(texture(texture_water,uv)).rgb,0.8);

    }else if(isWater==3){
        return (texture(texture_water,uv)).rgb;
    }else{
        if(height<0.3) {
        float borne_v_b = 0.3;
        float borne_b = 0.2;
        vec3 couleurTop = texture(texture_grass,uv).rgb;
        vec3 couleurMid = texture(texture_sand,uv).rgb;
        return vec3(couleurTop.x-(couleurTop.x-couleurMid.x)*(borne_b-height)/(borne_b-borne_v_b),
                    couleurTop.y-(couleurTop.y-couleurMid.y)*(borne_b-height)/(borne_b-borne_v_b),
                    couleurTop.z-(couleurTop.z-couleurMid.z)*(borne_b-height)/(borne_b-borne_v_b));
        }
        }
}

void main() {
    color = vec3(0.0,0.0,0.0); //DO NOT TOUCH OR IT WILL BLOW UP
    vec3 kd = get_kd_fBm();
    if(isWater!=0){
        kd =get_kd_water(kd);
    }
    color = kd;
}
