#version 330

in vec2 uv;

out vec4 color;
in float height;
in float isWater;
in float currentWaterLevel;

uniform sampler2D colorTex;
uniform sampler2D mirrorTex;
uniform sampler2D tex;
uniform sampler2D texture_snow;
uniform sampler2D texture_rock;
uniform sampler2D texture_grass;
uniform sampler2D texture_sand;
uniform sampler2D texture_water;
uniform sampler2D heightTex;
uniform sampler2D texture_bump;
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
    float borne_j_v = 0;
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
vec3 get_kd_water(vec3 texture_to_mix){
    if(isWater==1 || height<=currentWaterLevel){
        float window_width = textureSize(mirrorTex,0).x;
        float window_height = textureSize(mirrorTex,0).y;
        float _u = 1-(gl_FragCoord.x)/window_width;
        float _v = (gl_FragCoord.y)/window_height;

       vec2 uv_ = vec2(_u,_v);
       return (mix(texture(mirrorTex,uv_).rgb,(texture(texture_water,uv)).rgb,0.4));

    }else if(isWater==3){
        return vec3((texture(texture_water,(uv+mod(time,3)/10))).rgb);
    }
}

vec3 getAmbientTerm(){

    vec3 La = vec3(1.0f, 1.0f, 1.0f);
    vec3 ka = get_kd_fBm();
    if(isWater!=0){
        ka =get_kd_water(ka);
    }
    return ka*La/3;
}

vec3 getPos(vec2 pos){
    return vec3(pos.x,pos.y,texture(heightTex,pos).x);
}
vec3 getPosBump(vec2 pos){
    return vec3(pos.x,pos.y,(texture(texture_bump,pos).x + texture(texture_bump,pos).y)/30);
}


vec3 getDiffuseTerm(vec3 kd){
    //kd = couleur du matériel
    vec3 pos = (vec3(uv.x,uv.y,height));
    //use uv.x -1 +1
    //decaler de 1/textureSize
    vec3 normal;
    if(height>currentWaterLevel){
    float window_width = textureSize(heightTex,0).x;
    float window_height = textureSize(heightTex,0).y;
    vec3 dfdx = getPos(vec2(uv.x+1/window_width,uv.y)) - getPos(vec2(uv.x-1/window_width,uv.y));
    vec3 dfdy = getPos(vec2(uv.x,uv.y+1/window_height)) - getPos(vec2(uv.x,uv.y-1/window_height));
     normal = normalize(cross((dfdx),(dfdy)));
    }else{
        float window_width = textureSize(texture_bump,0).x;
        float window_height = textureSize(texture_bump,0).y;
        vec3 dfdx = getPosBump(vec2(uv.x+1/window_width,uv.y)) - getPosBump(vec2(uv.x-1/window_width,uv.y));
        vec3 dfdy = getPosBump(vec2(uv.x,uv.y+1/window_height)) - getPosBump(vec2(uv.x,uv.y-1/window_height));
         normal = normalize(cross((dfdx),(dfdy)));

     //   normal = vec3(0,0,1);
    }
    vec3 light_dir = normalize(vec3(1,-4,5));
  //  if(reflect!=0){
   //     light_dir = vec3(-light_dir.x, -light_dir.y, light_dir.z);
   // }
    //Direction de la lumiere
    vec3 Ld = (vec3(1,1,1));
    //Couleur de la lumiere
    return vec3(kd * dot(normal,light_dir) * Ld);
}

/*vec3 getSpecularTerm(vec3 lightDir, vec3 normal){

    vec3 Ls = vec3(1.0f, 1.0f, 1.0f);
    vec3 r = normal * 2 * dot(normal, lightDir) -lightDir;

    ks = vec3(0.8f, 0.8f, 0.8f);
    float alpha = 60.0f;

    return Is = ks * pow(dot(r, v), alpha)*Ls;
}*/



void main() {
    //color = vec4(0.0,0.0,0.0,0.0); //DO NOT TOUCH OR IT WILL BLOW UP
    if(reflect == 0){
    vec3 kd = get_kd_fBm();
    if(isWater!=0){
        kd =get_kd_water(kd);
    }
    color = vec4(getAmbientTerm()+getDiffuseTerm(vec3(kd)),1);
    color = vec4(texture(texture_water,uv).rgb,1);
    }else{
        vec4 kd = vec4(getAmbientTerm()+getDiffuseTerm(get_kd_fBm()),1);

        if(height<=waterLevel){
            kd = vec4(0);
        }
        color = (kd);

    }

}
