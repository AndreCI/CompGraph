#version 330 core


float rand(vec2 co){return (fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453));}

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
out vec3 color;
uniform sampler2D permTexture;

vec2 getRandomGradient(vec2 xy){
    float x = 8 * rand(xy);
   return vec2(cos(x),sin(x));
}
float smoothInterpolation(float v){
   return 6*v*v*v*v*v - 15*v*v*v*v + 10*v*v*v;//6*pow(v,5) - 15*pow(v,4) +10*pow(v,3);
   // return v*v*v-v*v;
}

float mix(float x, float y, float al){
    return (1-al)*x+al*y;
}

float perlinNoise(vec2 pos){
    vec2 bl = floor(pos);
    vec2 br = bl + vec2(1,0);
    vec2 ul = bl + vec2(0,1);
    vec2 ur = bl + vec2(1,1);

    vec2 a = pos - bl;
    vec2 b = pos - br;
    vec2 c = pos - ul;
    vec2 d = pos - ur;

    float s = dot(getRandomGradient(bl),a);
    float t = dot(getRandomGradient(br),b);
    float u = dot(getRandomGradient(ul),c);
    float v = dot(getRandomGradient(ur),d);

    float st = mix(s,t,smoothInterpolation(fract(pos.x)));
    float uv_f = mix(u,v,smoothInterpolation(fract(pos.x)));
    return mix(st,uv_f,smoothInterpolation(fract(pos.y)));

}

void main() {

    color = vec3(perlinNoise(uv*10)*0.5+0.5);
}
