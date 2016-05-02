#version 330 core



in vec2 uv;
uniform float h_fBm;
uniform float lacunarity_fBm;
uniform int octaves_fBm;
uniform float offset_fBm;
uniform float time;

out vec3 color;
//uniform sampler2D permTexture;

float rand(vec2 co){return (fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453));}

vec2 getRandomGradient(vec2 xy, float seed){
    float x = seed * rand(xy);
   return vec2(cos(x),sin(x));
}
float smoothInterpolation(float v){
   return 6*v*v*v*v*v - 15*v*v*v*v + 10*v*v*v;//6*pow(v,5) - 15*pow(v,4) +10*pow(v,3);
   // return v*v*v-v*v;
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



void main() {

     float globalSpeed = 0.3;
     float water = (waterNoise(uv,60,80,getRandomGradient(vec2(1,1),5),8*globalSpeed,1,time)); //Grosse vague de fond
     water += (waterNoise(uv*4,65,40,getRandomGradient(vec2(-1,-0.5),4),7*globalSpeed,1.6,time)); //vague moyenne a contre courant
     water += (waterNoise(uv/2,5,2,getRandomGradient(vec2(-1,-1),5),2*globalSpeed,10,time)); //petites vagues chaotique rare
     water+=(0.5);
     water = water /7;

     float fbm = (fBm(uv*3,h_fBm,lacunarity_fBm,octaves_fBm,offset_fBm));
     if(fbm>water){
         color = vec3(fbm);
     }else{
         color = vec3(water);
     }


}
