#version 330

in vec2 position;

out vec2 uv;
out float height;
out float isRiver;

uniform sampler2D riverTex;
uniform sampler2D heightTex;
uniform mat4 MVP;
uniform float time;

vec2 getNextRiverPoint(vec2 pos){
    float righti = 0;
    float rightj = 0;
    float height_ = 10;
    float cu_height = 10;
    vec2 cu_uv;
    float corr = 25;
    for(float i=-1;i<1;i+=0.5){
        for(float j = -1; j<1; j+=0.5){
            cu_uv = vec2(pos.x+i/corr, pos.y+j/corr);
            cu_height = ((texture(heightTex,cu_uv).x + texture(heightTex,cu_uv).y)/2);
            if(cu_height<height_){
                height_ = cu_height;
                righti=i;
                rightj=j;
            }
        }
    }
   // vec3 x = dFdx(pos);
  //  vec3 y = dFdy(pos);

   // float newX = pos.x - step*x;
    return vec2(pos.x+righti/corr,pos.y+rightj/corr);
}

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    /* TODO 1.2 : 2 Create a shader program for terrain rendering.
The vertex shader samples the height map texture an displaces the vertices according to the height.
(reuse/adapt from HW3)*/

   height = ((texture(heightTex,uv).x + texture(heightTex,uv).y)/2);

  // height =(texture(colorTex,uv).x + texture(colorTex,uv).y);

    vec3 pos_3d = vec3(position.x, height, -position.y);
    gl_Position = MVP * vec4(pos_3d, 1.0);

    bool isRiverTemp = false;
    vec2 river = vec2(0.5,0.5);
    for(int i =0; i<10; i++){
        if(uv.x>=river.x-0.02 && uv.x<=river.x+0.02 && uv.y>=river.y-0.02 && uv.y<=river.y+0.02){
            isRiverTemp=true;
        }
        river=getNextRiverPoint(river);

    }
    if(isRiverTemp){
        isRiver=1;
    }else{
        isRiver=0;
    }
}
