#version 330 core



in vec2 uv;
uniform float h_fBm;
uniform float lacunarity_fBm;
uniform int octaves_fBm;
uniform float offset_fBm;

out vec3 color;
uniform sampler2D permTexture;

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



void main() {
    color = vec3(fBm(uv*3,h_fBm,lacunarity_fBm,octaves_fBm,offset_fBm));

   // color = 3.5*(vec3(fBm(uv,1,2,4)));

}
