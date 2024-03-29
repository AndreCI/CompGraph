// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include "grid/grid.h"
#include "framebuffer.h"
#include "screenquad/screenquad.h"
#include "cube/cube.h"
#include "trackball.h"

Cube cube;
Grid grid;
FrameBuffer framebuffer_heightMap;
FrameBuffer framebuffer_mirror;
ScreenQuad screenquad;
float theta;
float theta_up;
int window_width = 800;
int window_height = 800;
float inertieDir; //direction of the move where we will apply the inertie (temp var)
float inertieMark; //inertie mark, so inertie will stop a time t = inertieMark + inertieDuration (temp var)
const float inertieDuration = 0; //duration of inertie, make it 1.2 to have some nice effect

float *heightMap = (float*)calloc(window_height*window_width,sizeof(float));

vec3 curvePoints[3];

using namespace glm;

mat4 projection_matrix;
mat4 view_matrix;
vec3 eye_;
vec3 center_;
vec3 up_;
int bezier=0;

GLfloat currenty ;

mat4 PerspectiveProjection(float fovy, float aspect, float near, float far) {
    mat4 projection = IDENTITY_MATRIX;
    float top = near*tan(fovy);
    float bottom = -top;
    float right = top*aspect;
    float left=-right;
    float S = 1/(tan(fovy/2));
    projection[0][0] = S;
    projection[1][1] = S;
    projection[2][2] = -(far) / (far - near);
    projection[3][2] = -(far * near) / (far - near);
    projection[2][3] = -1.0f;
    projection[3][3] = 0;
    return projection;
}

mat4 LookAt(vec3 eye, vec3 center, vec3 up) {
    vec3 z_cam = normalize(eye - center);
    vec3 x_cam = normalize(cross(up, z_cam));
    vec3 y_cam = cross(z_cam, x_cam);

    mat3 R(x_cam, y_cam, z_cam);
    R = transpose(R);

    mat4 look_at(vec4(R[0], 0.0f),
                 vec4(R[1], 0.0f),
                 vec4(R[2], 0.0f),
                 vec4(-R * (eye), 1.0f));
    return look_at;
}

int getHeight(float x, float y){
    framebuffer_heightMap.Bind();
    //glReadPixels(round(((x+1)/4.0)*window_width),(round(((3+y)/4.0)*window_height)),window_width,window_height,GL_RED,GL_FLOAT,heightMap);
    int index = floor(round(((x+1)/4.0)*window_width)+(round(((3+y)/4.0)*window_height)));
    framebuffer_heightMap.Unbind();
    return heightMap[index];
}

vec3 bezierCurve(vec3 P0,vec3 P1 , vec3 P2 ,float time) {
      float t= 1 - time;
      float x = t*t * P0.x + 2*t *time *P1.x + time*time *P2.x;
      float z = t*t * P0.z + 2*t*time * P1.z + time * time *P2.z;
      vec3 p = vec3(x,P0.y,z);
      return p;
    }
vec3 changeCenter(vec3 d) {
    return vec3(fmod(d.x+0.5,3),fmod(d.y-0.2,3),d.z);
}

void listVec(vec3 buff[] , float time) {
    for(int i = 0; i < 3; i+=3){
         vec3 p0 = buff[i];
         vec3 p1 = buff[i + 1];
         vec3 p2 = buff[i + 2];
         eye_ = bezierCurve(p0,p1,p2,fmod(time/20,1));
         //vec3 center = changeCenter(center_);
         view_matrix = lookAt(eye_,center_,up_);
    }
}


void moveView(float direction, bool userCalled){
    inertieDir = direction;

    float speed = 0.1;
    if(inertieDuration==0){ //useful to debugging, but not for final
        speed = 1;
    }
    if(userCalled){
        inertieMark = glfwGetTime()+ inertieDuration;
    }
    //WASD QE is assumed with swiss keyboard
    if(direction==0){ //W
        vec2 ce = normalize(vec2(center_.x - eye_.x,center_.z - eye_.z));
        vec3 dir = vec3(ce.x,0,ce.y);
         eye_ = eye_+dir*vec3(0.05*speed);
         center_ = center_ + dir*vec3(0.05*speed);
    }else if(direction==1){ //S
        vec2 ce = normalize(vec2(eye_.x - center_.x,eye_.z - center_.z));
        vec3 dir = vec3(ce.x,0,ce.y);
        // vec3 dir = normalize(eye_ - center_);
         eye_ = eye_+dir*vec3(0.05*speed);
         center_ = center_ + dir*vec3(0.05*speed);
    }else if(direction==2){ //A
        float r = sqrt((eye_.x-center_.x)*(eye_.x-center_.x) + (eye_.z - center_.z)*(eye_.z - center_.z));
        theta = theta+0.02*speed;
        center_ = vec3(eye_.x + r*cos(theta),center_.y,eye_.z+r*sin(theta));
    }else if(direction==3){ //D
        float r = sqrt((eye_.x-center_.x)*(eye_.x-center_.x) + (eye_.z - center_.z)*(eye_.z - center_.z));
        theta = theta-0.02*speed;
        center_ = vec3(eye_.x + r*cos(theta),center_.y,eye_.z+r*sin(theta));
    }else if(direction==4){ //Q
        float r = sqrt((eye_.y - center_.y)*(eye_.y - center_.y)+(eye_.x-center_.x)*(eye_.x-center_.x) +(eye_.z - center_.z)*(eye_.z - center_.z));
        float vnorm = sqrt((eye_.x-center_.x)*(eye_.x-center_.x) +(eye_.z - center_.z)*(eye_.z - center_.z));
        theta_up = theta_up + 0.01*speed;
        vec3 v = vec3(center_.x-eye_.x,0, center_.z-eye_.z)/vnorm;
        center_ = vec3(eye_.x + v.x*r*abs(cos(theta_up)), eye_.y + r*abs(sin(theta_up)),eye_.z + v.z*r*abs(cos(theta_up)));
    }else if(direction==5){ //E
        float r = sqrt((eye_.y - center_.y)*(eye_.y - center_.y)+(eye_.x-center_.x)*(eye_.x-center_.x) +(eye_.z - center_.z)*(eye_.z - center_.z));
        float vnorm = sqrt((eye_.x-center_.x)*(eye_.x-center_.x) +(eye_.z - center_.z)*(eye_.z - center_.z));
        theta_up = theta_up - 0.01*speed;
        vec3 v = vec3(center_.x-eye_.x,0, center_.z-eye_.z)/vnorm;
        center_ = vec3(eye_.x + v.x*r*abs(cos(theta_up)), eye_.y + r*abs(sin(theta_up)),eye_.z + v.z*r*abs(cos(theta_up)));
    }else if(direction==6){ //O
        eye_=vec3(eye_.x,eye_.y+0.05*speed,eye_.z);
        center_ = vec3(center_.x,center_.y+0.05*speed,center_.z);
    }else if(direction==7) { //P
        eye_=vec3(eye_.x,eye_.y-0.05*speed,eye_.z);
        center_ = vec3(center_.x,center_.y-0.05*speed,center_.z);
    }
    center_ = vec3(center_.x,0.5f , center_.z);
    up_ = vec3(0.0f, 1.0f, 0.0f);
     cout << "Vous etes a : " << eye_.x << " x ; " << eye_.z << " z ; " << eye_.y << " y ; " << endl;
     //cout <<"Vous regardez le point : " << center_.x<< " " << center_.z<< " " << center_.y << endl;
   view_matrix = LookAt(eye_,center_,up_);
}




int fillRiverPoints(float *riverPoints, int size, vec2 head){
    if(size%2==0 && riverPoints){
    float riverx = head.x;
    float rivery = head.y;
    float corr = 20;
    float right_i = 0;
    float right_j = 0;
    float looking_height = 10;
    float cu_height = 10;

    for(int k=0; k<floor(size/2); k++){

        for(float i=-1;i<=1;i+=0.5){
            for(float j = -1; j<=1; j+=0.5){
                looking_height = getHeight(riverx+i/corr,rivery+j/corr);
                if(looking_height<cu_height){
                    cu_height = looking_height;
                    right_i = i;
                    right_j = j;
                }
            }
        }
        riverx = riverx + right_i/corr;
        rivery = rivery + right_j/corr;
        riverPoints[k*2] = riverx;
        riverPoints[k*2+1] = rivery;
    }
         return 0;
    }else{
         return -1;
    }
}

int fillAllRivers(vec2 *headList, int *lengths, int sizeOfHeads, float *riverPoints){
    int a = 0;
    for(int i = 0;i<sizeOfHeads;i++){
        riverPoints[a] = lengths[i];
        riverPoints[a+1] = lengths[i];
        if(fillRiverPoints(riverPoints+a+2, riverPoints[a]*2, headList[i])==-1){
            return -1;
        }
        a = a + 2 + riverPoints[a]*2;
    }
    return 0;
}

void Init() {
    // sets background color
    GLuint noise_tex_id;
    GLuint mirror_tex_id;
    noise_tex_id = framebuffer_heightMap.Init(window_width,window_height, true);
    mirror_tex_id = framebuffer_mirror.Init(window_width,window_height);
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);

    eye_ = vec3(-2.0f, 2.2f, 2.0f);
    center_ = vec3(1.0f, 0.5f, -1.0f);
    up_ = vec3(0,1,0);
    theta =-acos((center_.x -eye_.x)/sqrt((eye_.x-center_.x)*(eye_.x-center_.x) +(eye_.y-center_.y)*(eye_.y-center_.y)+(eye_.z - center_.z)*(eye_.z - center_.z)));
    if(eye_.z<center_.z && eye_.x>center_.x) {
        theta = -theta;
    }
    theta_up =-asin((center_.y -eye_.y)/sqrt((eye_.x-center_.x)*(eye_.x-center_.x) +(eye_.y-center_.y)*(eye_.y-center_.y)+(eye_.z - center_.z)*(eye_.z - center_.z)));

    view_matrix = lookAt(eye_,center_,up_);
    screenquad.Init(window_width,window_height,noise_tex_id);
    cube.Init();
   // enable depth test.
    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    framebuffer_heightMap.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    screenquad.Draw();
    framebuffer_heightMap.Unbind();

    int sizeOfHeadlist = 3;
    vec2 headList[3]; // MUST BE = sizeOfHeadlist
    headList[0] = vec2(0.93,0.42);
    headList[1] = vec2(0.7,0.6);
    headList[2] = vec2(0.36,0.39);
    int lenghts[3]; //MUST BE = sizeOfHeadlist
    lenghts[0] = 3;
    lenghts[1] = 3;
    lenghts[2] = 2;


    int riverPointsSize = sizeOfHeadlist*2;
    for(int i =0; i<sizeOfHeadlist; i++){
        riverPointsSize+=lenghts[i]*2;
    }
    float *riverPoints = (float*)calloc(riverPointsSize+3,sizeof(float*));
    fillAllRivers(headList,lenghts,sizeOfHeadlist,riverPoints);
        grid.Init(noise_tex_id,mirror_tex_id,256, riverPoints, riverPointsSize);
    free(riverPoints);

    curvePoints[0] = vec3(-2,2.5,-1.5);
    curvePoints[1] = vec3(1.5,2.5,2);
    curvePoints[2] = vec3(5,2.5,-1.5);

}


// gets called for every frame.
void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const float time = glfwGetTime();
    mat4 scaledModel = scale(IDENTITY_MATRIX,vec3(2.0,1.5,2.0));

    if(inertieMark>time){
        moveView(inertieDir,false);
    }

    //  mirror the camera position
    float waterLevel = 0.2;
    vec3 cam_pos_mirror = vec3(eye_.x,0.8-eye_.y,eye_.z);
    vec3 center_mirror = vec3(center_.x,-center_.y, center_.z);
    // create new VP for mirrored camera
    vec3 up_mirror = vec3(0,1,0);
    mat4 view_mirror = lookAt(cam_pos_mirror,center_,-up_mirror);

   framebuffer_mirror.Bind();
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   cube.Draw(IDENTITY_MATRIX,view_mirror,projection_matrix);
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   grid.Draw(time, scaledModel, view_mirror, projection_matrix,1,waterLevel);
   glDisable(GL_CULL_FACE);
   framebuffer_mirror.Unbind();

   grid.Draw(time,scaledModel, view_matrix, projection_matrix,0,waterLevel);
   cube.Draw(IDENTITY_MATRIX,view_matrix,projection_matrix);
   //eye_ = vec3(eye_.x,getHeight(eye_.x,eye_.z) + 0.1f,eye_.z);
   if(bezier) {
   listVec(curvePoints,time);
   }
   view_matrix = lookAt(eye_,center_,up_);
}

// transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 TransformScreenCoords(GLFWwindow* window, int x, int y) {
    // the framebuffer and the window doesn't necessarily have the same size
    // i.e. hidpi screens. so we need to get the correct one
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    return vec2(2.0f * (float)x / width - 1.0f,
                1.0f - 2.0f * (float)y / height);
}



// Gets called when the windows/framebuffer is resized.
void SetupProjection(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    cout << "Window has been resized to "
         << window_width << "x" << window_height << "." << endl;

    glViewport(0, 0, window_width, window_height);

     projection_matrix = PerspectiveProjection(45.0f,
                                               (GLfloat)window_width / window_height,
                                               0.1f, 100.0f);
     GLfloat top = 1.0f;
     GLfloat right = (GLfloat)window_width / window_height * top;

}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if(action == GLFW_PRESS || action == GLFW_REPEAT){
    bool inertie = action == GLFW_REPEAT;
    switch(key){
    case 'W':
           cout<<"Moving forward"<<endl;
           moveView(0,true);
           break;
       case 'S':
           cout<<"Moving backward"<<endl;
           moveView(1,true);
           break;

       case 'A':
           cout<<"Moving left"<<endl;
           moveView(3,true);
           break;
       case 'D':
           cout<<"Moving right"<<endl;
           moveView(2,true);
           break;
       case 'Q':
           cout<<"Looking Up"<<endl;
           moveView(4,true);
           break;
       case 'E':
           cout<<"Looking Down"<<endl;
           moveView(5,true);
           break;
       case 'O':
           moveView(6,true);
           break;
       case 'P':
            moveView(7,true);
            break;
       case 'B' :
            if(bezier) {
            bezier =0;
            } else {
            bezier = 1;
            }
            break;
        }
    }
  }



int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(window_width, window_height,
                                          "Project Mountain", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, SetupProjection);


    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init();

    // update the window size with the framebuffer size (on hidpi screens the
    // framebuffer is bigger)
    glfwGetFramebufferSize(window, &window_width, &window_height);
    SetupProjection(window, window_width, window_height);

    // render loop
    while(!glfwWindowShouldClose(window)){
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    grid.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
