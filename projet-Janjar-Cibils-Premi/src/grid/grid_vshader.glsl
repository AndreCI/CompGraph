#version 330

in vec2 position;

out vec2 uv;
out float height;
out float isWater;
uniform sampler2D riverTex;
uniform sampler2D heightTex;
uniform mat4 MVP;
uniform float time;


float rand(vec2 co){return (fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453));}
vec2 getRandomGradient(vec2 xy, float seed){
    float x = seed * rand(xy);
   return vec2(sin(x),cos(x));
}

float waterNoise(vec2 pos, float amplitude, float wavelenght, vec2 direction, float speed, float waveNumber,float time_v){
    float q = 1/(waveNumber*amplitude);
    float phase = speed*2*3.1415/wavelenght;
    float z = q*amplitude * direction.y*cos(waveNumber*dot(direction,pos)+phase*time_v);
    return z;
}

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    /* TODO 1.2 : 2 Create a shader program for terrain rendering.
The vertex shader samples the height map texture an displaces the vertices according to the height.
(reuse/adapt from HW3)*/


    float globalSpeed = 0.3;
    float water = (waterNoise(uv,60,80,getRandomGradient(vec2(1,1),5),8*globalSpeed,1,time)); //Grosse vague de fond
    water += (waterNoise(uv*4,65,40,getRandomGradient(vec2(-1,-0.5),4),7*globalSpeed,1.6,time)); //vague moyenne a contre courant
    water += (waterNoise(uv/2,5,2,getRandomGradient(vec2(-1,-1),5),2*globalSpeed,10,time)); //petites vagues chaotique rare
    water+=(0.5);
    water = water/8;

    bool waterDefined = false;
   height = ((texture(heightTex,uv).x + texture(heightTex,uv).y)/2);
   if(height<water){
       height = (water);
       isWater=1;
       waterDefined=true;
   }

    if(texture(riverTex,uv).x==1){
        height  = height +water/6;
        isWater=1;
    }else{
        if(!waterDefined){
          isWater=0;
        }
    }

    vec3 pos_3d = vec3(position.x, height, -position.y);
    gl_Position = MVP * vec4(pos_3d, 1.0);
}
