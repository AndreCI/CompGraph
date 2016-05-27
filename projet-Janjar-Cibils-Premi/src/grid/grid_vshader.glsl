#version 330


in vec2 position;

out vec2 uv;
out float currentWaterLevel;
out float height;
out float isWater;
uniform sampler2D heightTex;
uniform mat4 MVP;
mat4 scale;
uniform float time;
uniform vec2 riverPoints[100];
uniform int riverPointsSize;
uniform float waterLevel;

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

float distance(vec2 a, vec2 b){
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.25; //0->1 into -1 -> 2

    bool waterDefined = false;
   height = (texture(heightTex,uv).x); //RED Channel has the value in it
   currentWaterLevel = waterLevel;
   if(height<=waterLevel){
       isWater=1;
       height = waterNoise(uv,0.1,1,uv,0.2,1,time)/10;
       height +=waterNoise(uv, 0.4,1.2,vec2(uv.x+4,uv.y-5),0.2,3,time)/10;
       height +=waterNoise(uv, 0.8,0.8,vec2(uv.x+2,uv.y-6),0.4,1,time)/10;
       height +=waterNoise(uv, 0.6,0.2,vec2(uv.x+9,uv.y-1),0.3,5,time)/10;
       height +=waterNoise(uv, 0.8,0.2,vec2(uv.x+2,uv.y-4),0.1,3,time)/10;
       height = height/100;
       float noiseheight = waterNoise(uv,1,1,vec2(uv.x+1,uv.y-2),1,90,time);
       noiseheight += waterNoise(uv,2,4,vec2(uv.x-1.5,uv.y+1),1,78,time);
       noiseheight += waterNoise(uv,0.5,2,vec2(uv.x-3,uv.y-3),1,50,time);
       noiseheight = noiseheight/5;

       height=height + noiseheight + waterLevel-0.05;
    //   height += (texture(texture_bump,uv + vec2(cos(time_slowed)/2+0.5, sin(time_slowed)/4 +0.25)).y + texture(texture_bump, vec2(uv.y,uv.x)+ vec2(cos(time_slowed)/2+0.5, sin(time_slowed)/4 +0.25)).x)/20;
       currentWaterLevel = height;
       waterDefined=true;    
   }
   float epsilon = 0.005;
   int tempidx = 0;
   float end = riverPoints[0].x-1;
   bool writing = true;
   for(int a = 1; a<riverPointsSize; a++){
       if(writing){
           tempidx++;
           if(distance(uv,riverPoints[a]) + distance(riverPoints[a+1],uv) - distance(riverPoints[a],riverPoints[a+1])<epsilon
                   && riverPoints[a] !=vec2(0,0)){

             isWater = 3;
             height = height;
             waterDefined=true;

           }
           if(tempidx==end){
               writing = false;
               a++;
           }
       }else{
           end = riverPoints[a].x-1;
           writing = true;
           tempidx=0;
       }

   }
    if(!waterDefined){
      isWater=0;
    }

    vec3 pos_3d = vec3(position.x, height, -position.y);



    // setup MVP
    scale[0][0] = 2.8f;
    scale[1][1] = 1.7f;
    scale[2][2] = 2.8f;
    scale[3][3] = 1.0f;

    gl_Position = MVP*vec4(pos_3d, 1.0);
}
