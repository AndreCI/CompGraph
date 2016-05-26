#version 330

in vec2 uv;

out vec4 color;
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
uniform float time;
uniform float reflect;
uniform float waterLevel;



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

    return vec3(kd);
}

vec3 getAmbientTerm(){

    vec3 La = vec3(1.0f, 1.0f, 1.0f);
    vec3 ka = vec3(0.18f, 0.1f, 0.1f);
    return ka*La;
}

vec3 getDiffuseTerm(vec3 kd){
    //kd = couleur du matériel
    vec3 pos = (vec3(uv.x,uv.y,height));
    //use uv.x -1 +1
    //decaler de 1/textureSize
    vec3 Y = normalize(dFdy(pos));
    vec3 X = normalize(dFdx(pos));
    vec3 normal = (cross(X,Y));
    vec3 light_dir = normalize(vec3(10,10,40));
    //Direction de la lumiere
    vec3 Ld = (vec3(1,1,1));
    //Couleur de la lumiere
    return vec3(kd * dot(normal,light_dir) * Ld * 1.5);
}

/*vec3 getSpecularTerm(vec3 lightDir, vec3 normal){

    vec3 Ls = vec3(1.0f, 1.0f, 1.0f);
    vec3 r = normal * 2 * dot(normal, lightDir) -lightDir;

    ks = vec3(0.8f, 0.8f, 0.8f);
    float alpha = 60.0f;

    return Is = ks * pow(dot(r, v), alpha)*Ls;
}*/

vec3 get_kd_water(vec3 texture_to_mix){
    if(isWater==1 || height<waterLevel){
        float window_width = textureSize(mirrorTex,0).x;
        float window_height = textureSize(mirrorTex,0).y;
        float _u = 1-(gl_FragCoord.x)/window_width;
        float _v = (gl_FragCoord.y)/window_height;

       vec2 uv_ = vec2(_u,_v);
       return (mix(texture(mirrorTex,uv_).rgb,(texture(texture_water,uv)).rgb,0));

    }else if(isWater==3){
        return vec3((texture(texture_water,(uv+mod(time,3)/10))).rgb);
    }else{
        if(height<waterLevel+0.1) {
        float borne_v_b = waterLevel+0.1;
        float borne_b = waterLevel;
        vec3 couleurTop = texture(texture_grass,uv).rgb;
        vec3 couleurMid = texture(texture_sand,uv).rgb;
        return vec3(couleurTop.x-(couleurTop.x-couleurMid.x)*(borne_b-height)/(borne_b-borne_v_b),
                    couleurTop.y-(couleurTop.y-couleurMid.y)*(borne_b-height)/(borne_b-borne_v_b),
                    couleurTop.z-(couleurTop.z-couleurMid.z)*(borne_b-height)/(borne_b-borne_v_b)
                    );
        }
        }
}

void main() {
    //color = vec4(0.0,0.0,0.0,0.0); //DO NOT TOUCH OR IT WILL BLOW UP
    if(reflect == 0){
    vec3 kd = get_kd_fBm();
    if(isWater!=0){
        kd =get_kd_water(kd);
    }
    color = vec4(kd,1);//vec4(getAmbientTerm() + getDiffuseTerm(vec3(kd)),1);
    }else{
        vec4 kd = vec4(get_kd_fBm(),1);
        /*if(isWater!=0){
            kd =get_kd_water(kd);
        }*/

      //  if(height<=waterLevel){
       //     kd = vec4(0);
       // }
        color = kd;

    }
}
