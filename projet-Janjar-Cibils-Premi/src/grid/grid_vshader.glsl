#version 330

in vec2 position;

out vec2 uv;
out float height;
out float isWater;
uniform sampler2D riverTex;
uniform sampler2D heightTex;
uniform mat4 MVP;
mat4 scale;
uniform float time;

bool isNextToLand_f(vec2 pos){
    float corr = 10;
    for(int i=-1;i<1;i++){
        for(int j=-1;j<1;j++){
            if(texture(riverTex,uv+vec2(i/corr,j/corr)).x==0){
                return true;
            }
        }
    }
    return false;
}

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

  /*  float globalSpeed = 1;
    //float water = (waterNoise(uv,25,10,getRandomGradient(vec2(1,1),5),8*globalSpeed,6,time)); //Grosse vague de fond
    float water = (waterNoise(uv*4,15,20,getRandomGradient(vec2(-1,-0.5),4),7*globalSpeed,4,time)); //vague moyenne a contre courant
    water += (waterNoise(uv/2,5,2,getRandomGradient(vec2(-1,-1),5),2*globalSpeed,10,time)); //petites vagues chaotique rare
    water+=(0.6);
    water = water/16;*/

   float water = 0.2;
    bool waterDefined = false;
   height = ((texture(heightTex,uv/2).x + texture(heightTex,uv/2).y)/2);
   if(height<water){
       height = (water);
       isWater=1;
       waterDefined=true;    
   }else if(height-0.2<water){
        isWater=2;
   }

    if(texture(riverTex,uv/2).x==1){
        height  = height +water/6;
        isWater=1;
    }else{
        if(!waterDefined){
          isWater=0;
        }
    }
   // height = water;
    //isWater = 1;

    vec3 pos_3d = vec3(position.x, height, -position.y);



    // setup MVP
    scale[0][0] = 3.0f;
    scale[1][1] = 3.0f;
    scale[2][2] = 3.0f;
    scale[3][3] = 1.0f;

    gl_Position = MVP*scale * vec4(pos_3d, 1.0);
}
