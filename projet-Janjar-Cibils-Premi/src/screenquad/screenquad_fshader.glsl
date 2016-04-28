#version 330 core


float rand(vec2 co){return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);}

/*float rand (vec2 co, float l) {return rand(vec2(rand(co), l));}

float rand (vec2 co, float l, float t) {return rand(vec2(rand(co, l), t));}

float perlin(vec2 p, float dim, float time) {
    vec2 pos = floor(p * dim);
    vec2 posx = pos + vec2(1.0, 0.0);
    vec2 posy = pos + vec2(0.0, 1.0);
    vec2 posxy = pos + vec2(1.0);

    float c = rand(pos, dim, time);
    float cx = rand(posx, dim, time);
    float cy = rand(posy, dim, time);
    float cxy = rand(posxy, dim, time);

    vec2 d = fract(p * dim);
    d = -0.5 * cos(d * 3.14159265358979323846) + 0.5;

    float ccx = mix(c, cx, d.x);
    float cycxy = mix(cy, cxy, d.x);
    float center = mix(ccx, cycxy, d.y);

    return center * 2.0 - 1.0;
}

// p must be normalized!
float perlin(vec2 p, float dim) {
    vec2 pos = floor(p * dim);
    vec2 posx = pos + vec2(1.0, 0.0);
    vec2 posy = pos + vec2(0.0, 1.0);
    vec2 posxy = pos + vec2(1.0);

    // For exclusively black/white noise
    float c = step(rand(pos, dim), 0.5);
    float cx = step(rand(posx, dim), 0.5);
    float cy = step(rand(posy, dim), 0.5);
    float cxy = step(rand(posxy, dim), 0.5);

    return perlin(p, dim, 0.0);
}*/
in vec2 uv;
out float color;
uniform sampler2D permTexture;

vec2 getLowerPoint(vec2 pos, int nbrCell){
    return  floor(pos*nbrCell)/nbrCell;
}

vec2 getRandomGradient(){
   return vec2(rand(uv),rand(uv));
}
float smoothInterpolation(float v){
    return 6*pow(v,5) - 15*pow(v,4) +10*pow(v,3);
}

float mix(float x, float y, float al){
    return (1-al)*x+al*y;
}

float perlinNoise(vec2 uv, int nbrCell){
    float s = dot(getRandomGradient(),getLowerPoint(uv, nbrCell) - uv);
    float t = dot(getRandomGradient(),getLowerPoint(uv,nbrCell)-uv + vec2(0,1/nbrCell) - uv);
    float u = dot(getRandomGradient(),getLowerPoint(uv,nbrCell)-uv + vec2(1/nbrCell,0) - uv);
    float v = dot(getRandomGradient(),getLowerPoint(uv,nbrCell)-uv + vec2(1/nbrCell,1/nbrCell)- uv);

    float st = mix(s,t,smoothInterpolation(uv.x));
    float uv_f = mix(u,v,smoothInterpolation(uv.x));
    return mix(st,uv_f,smoothInterpolation(uv.y));
}




void main() {

    color = perlinNoise(uv, 10);
}
