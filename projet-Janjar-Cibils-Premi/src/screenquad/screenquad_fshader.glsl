#version 330 core
in vec2 uv;
float h_fBm=1.2;
float lacunarity_fBm=3.7;
int octaves_fBm=12;
float offset_fBm=0.32;
float time;

layout (location = 0) out float heightMap;
layout (location = 1) out float lavaMap;

float rand(vec2 co){return (fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453));}

vec2 getRandomGradient(vec2 xy, float seed){
    float x = seed * rand(xy);
   return vec2(cos(x),sin(x));
}
float smoothInterpolation(float v){
   return 6*v*v*v*v*v - 15*v*v*v*v + 10*v*v*v;
}

float mix(float x, float y, float al){
    return (1-al)*x+al*y;
}

float perlinNoise(vec2 pos,float seed){
    vec2 bl = floor(pos);
    vec2 br = bl + vec2(1,0);
    vec2 ul = bl + vec2(0,1);
    vec2 ur = bl + vec2(1,1);

    vec2 a = pos - bl;
    vec2 b = pos - br;
    vec2 c = pos - ul;
    vec2 d = pos - ur;

    float s = dot(getRandomGradient(bl, seed),a);
    float t = dot(getRandomGradient(br, seed),b);
    float u = dot(getRandomGradient(ul,seed),c);
    float v = dot(getRandomGradient(ur,seed),d);

    float st = mix(s,t,smoothInterpolation(fract(pos.x)));
    float uv_f = mix(u,v,smoothInterpolation(fract(pos.x)));
    return mix(st,uv_f,smoothInterpolation(fract(pos.y)));

}

float mask(float v, float t){
    if(v<t){
        return 0;
    }else{
        return 1;
    }
}
float fBm_lava(vec2 pos, float h, float l, int octaves, float offset){
float v = 0;
vec2 p = pos;
for(int i =0; i<octaves;i++){
v += mask(abs(((perlinNoise(p,10*i))+offset)*pow(l,-h)),0.1);
// p *=l;
// h-=0.2;
}
return v;
}

float fBm(vec2 pos, float h, float l, int octaves, float offset){
    float v = 0;
    vec2 p = pos;
    for(int i =0; i<octaves;i++){
        v += ((perlinNoise(p,10)+offset)*pow(l,-h*i));
        p *=l;
       // h-=0.2;
    }
    return v;
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
        heightMap = (fBm(uv*3,h_fBm,lacunarity_fBm,octaves_fBm,offset_fBm));
        lavaMap =  1- fBm_lava(uv*20,0.3,9.25,2,0.02);
}





