#version 330


in vec2 position;

out vec2 uv;
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
   if(height<=waterLevel){
  //     height = (waterLevel);
       isWater=1;
       waterDefined=true;    
   }else if((height-rand(uv)/15)<waterLevel){
        isWater=2;
        waterDefined=true;
   }
   float epsilon = 0.005;
 //  riverPoints[5] = vec2(0.42,0.6);
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
   /* scale[0][0] = 3.0f;
    scale[1][1] = 3.0f;
    scale[2][2] = 3.0f;
    scale[3][3] = 1.0f;*/

    gl_Position = MVP* vec4(pos_3d, 1.0);
}
