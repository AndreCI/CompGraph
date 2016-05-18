// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "grid/grid.h"
#include "framebuffer.h"
#include "trackball.h"
#include "screenquad/screenquad.h"
#include "cube/cube.h"
Cube cube;
Grid grid;
FrameBuffer framebuffer;
FrameBuffer water_framebuffer;
ScreenQuad screenquad;

int window_width = 800;
int window_height = 600;

using namespace glm;

float theta;
float theta_up;
vec3 eye_;
vec3 center_;
vec3 up_;
mat4 projection_matrix;
mat4 view_matrix;
mat4 trackball_matrix;
mat4 old_trackball_matrix;

mat4 quad_model_matrix;

GLfloat currenty ;

Trackball trackball;

struct heightMap{
    float BottomLeft[30000];
    float BottomMid[30000];
    float BottomRight[30000];
    float MidLeft[30000];
    float MidMid[30000];
    float MidRight[30000];
    float UpLeft[30000];
    float UpMid[30000];
    float UpRight[30000];

} heightMap;


mat4 OrthographicProjection(float left, float right, float bottom,
                            float top, float near, float far) {
    assert(right > left);
    assert(far > near);
    assert(top > bottom);
    mat4 projection = mat4(1.0f);
    projection[0][0] = 2.0f / (right - left);
    projection[1][1] = 2.0f / (top - bottom);
    projection[2][2] = -2.0f / (far - near);
    projection[3][3] = 1.0f;
    projection[3][0] = -(right + left) / (right - left);
    projection[3][1] = -(top + bottom) / (top - bottom);
    projection[3][2] = -(far + near) / (far - near);
    return projection;
}

mat4 PerspectiveProjection(float fovy, float aspect, float near, float far) {
    // TODO 1: Create a perspective projection matrix given the field of view,
    // aspect ratio, and near and far plane distances.
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

void Init() {
    // sets background color
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);

    GLuint noise_tex_id;
    GLuint river_tex_id;
    std::tie(noise_tex_id, river_tex_id) = framebuffer.Init(window_width,window_height);
    grid.Init(noise_tex_id, river_tex_id,256);
    screenquad.Init(window_width,window_height,noise_tex_id);
    cube.Init();
    // enable depth test.
    glEnable(GL_DEPTH_TEST);

    // TODO 3: once you use the trackball, you should use a view matrix that
    // looks straight down the -z axis. Otherwise the trackball's rotation gets
    // applied in a rotated coordinate frame.
    // uncomment lower line to achieve this.
    eye_ = vec3(-0.5f, 1.0f, -0.5f);
    center_ = vec3(0.0f, 1.0f, 0.0f);
    up_ = vec3(0,1,0);
    theta = 3.1415/4;
    theta_up = -3.1415/4;
    float r = sqrt((eye_.x-center_.x)*(eye_.x-center_.x) + (eye_.y-center_.y)*(eye_.y-center_.y) + (eye_.z - center_.z)*(eye_.z - center_.z));
    center_ = vec3(eye_.x + r*cos(theta),0,eye_.z+r*sin(theta));
    view_matrix = LookAt(eye_,
                         center_,
                        up_);

//     view_matrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, -4.0f));

    trackball_matrix = IDENTITY_MATRIX;

    // scaling matrix to scale the cube down to a reasonable size.

    quad_model_matrix = translate(mat4(1.0f), vec3(0.0f, -0.25f, 0.0f));


    framebuffer.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     screenquad.Draw();
     glReadPixels(0,0,650,1,GL_RED,GL_FLOAT,heightMap.BottomLeft);
    framebuffer.Unbind();
    cout << heightMap.BottomLeft[500] <<endl;
}

// gets called for every frame.
void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const float time = glfwGetTime();


    cube.Draw(trackball_matrix*quad_model_matrix,view_matrix,projection_matrix);
    grid.Draw(time, trackball_matrix * quad_model_matrix, view_matrix, projection_matrix);
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

void MouseButton(GLFWwindow* window, int button, int action, int mod) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        vec2 p = TransformScreenCoords(window, x_i, y_i);
        currenty = p.y;
        trackball.BeingDrag(p.x, p.y);
        old_trackball_matrix = trackball_matrix;
        // Store the current state of the model matrix.
        //zoom = p.y;
    }
}

void MousePos(GLFWwindow* window, double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        vec2 p = TransformScreenCoords(window, x, y);
        // TODO 3: Calculate 'trackball_matrix' given the return value of
        // trackball.Drag(...) and the value stored in 'old_trackball_matrix'.
        // See also the mouse_button(...) function.
        // trackball_matrix = ...
         trackball_matrix = trackball.Drag(p.x,p.y);

         mat4 t = trackball.Drag(p.x,p.y);

         trackball_matrix = t*old_trackball_matrix;
    }

    // zoom
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        // TODO 4: Implement zooming. When the right mouse button is pressed,
        // moving the mouse cursor up and down (along the screen's y axis)
        // should zoom out and it. For that you have to update the current
        // 'view_matrix' with a translation along the z axis.
        // view_matrix = ...

        vec2 p = TransformScreenCoords(window, x, y);

        if(p.y>currenty){
            view_matrix = translate(view_matrix,vec3(0.0f, 0.0f, 1.0f));
        }else{
            view_matrix = translate(view_matrix,vec3(-0.0f, -0.0f, -1.0f));
        }

        currenty = p.y;
    }
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


void moveView(float direction){
    //WASD QE is assumed with swiss keyboard
    if(direction==0){ //W
        vec3 dir = normalize(center_- eye_);
         eye_ = eye_+dir*vec3(0.1);
    }else if(direction==1){ //S
        vec3 dir = normalize(eye_ - center_);
         eye_ = eye_+dir*vec3(0.1);
    }else if(direction==2){ //A
        float r = sqrt((eye_.x-center_.x)*(eye_.x-center_.x) + (eye_.z - center_.z)*(eye_.z - center_.z));
        theta = theta+0.02;
        center_ = vec3(eye_.x + r*cos(theta),center_.y,eye_.z+r*sin(theta));
    }else if(direction==3){ //D
        float r = sqrt((eye_.x-center_.x)*(eye_.x-center_.x) + (eye_.z - center_.z)*(eye_.z - center_.z));
        theta = theta-0.02;
        center_ = vec3(eye_.x + r*cos(theta),center_.y,eye_.z+r*sin(theta));
    }else if(direction==4){ //Q
        float r = sqrt((eye_.y - center_.y)*(eye_.y - center_.y)+(eye_.x-center_.x)*(eye_.x-center_.x) +(eye_.z - center_.z)*(eye_.z - center_.z));
        float vnorm = sqrt((eye_.x-center_.x)*(eye_.x-center_.x) +(eye_.z - center_.z)*(eye_.z - center_.z));
        theta_up = theta_up + 0.02;
        vec3 v = vec3(center_.x-eye_.x,0, center_.z-eye_.z)/vnorm;
        center_ = vec3(eye_.x + v.x*r*cos(theta_up), eye_.y + r*sin(theta_up),eye_.z + v.z*r*cos(theta_up));
    }else if(direction==5){ //E
        float r = sqrt((eye_.y - center_.y)*(eye_.y - center_.y)+(eye_.x-center_.x)*(eye_.x-center_.x) +(eye_.z - center_.z)*(eye_.z - center_.z));
        float vnorm = sqrt((eye_.x-center_.x)*(eye_.x-center_.x) +(eye_.z - center_.z)*(eye_.z - center_.z));
        theta_up = theta_up - 0.02;
        vec3 v = vec3(center_.x-eye_.x,0, center_.z-eye_.z)/vnorm;
        center_ = vec3(eye_.x + v.x*r*cos(theta_up), eye_.y + r*sin(theta_up),eye_.z + v.z*r*cos(theta_up));
    }

    up_ = vec3(0.0f, 1.0f, 0.0f);

     view_matrix = LookAt(eye_,
                         center_,
                         up_);
}

void reDrawNoise(){
    framebuffer.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    screenquad.Draw();
    framebuffer.Unbind();
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    switch(key){
        case 'O':
            cout << "H increased (fBm)" << endl;
            screenquad.updateH(true);
            reDrawNoise();
             break;
        case 'P':
        cout << "H decreased (fBm)" << endl;
        screenquad.updateH(false);
        reDrawNoise();
        break;
    case 'I':
        cout << "Lacunarity increased (fBm)" << endl;
        screenquad.updateLacunarity(true);
        reDrawNoise();
         break;
    case 'U':
    cout << "Lacunarity decreased (fBm)" << endl;
    screenquad.updateLacunarity(false);
    reDrawNoise();
    break;case 'M':
        cout << "Octaves increased (fBm)" << endl;
        screenquad.updateOctaves(true);
        reDrawNoise();
         break;
    case 'L':
    cout << "Octaves decreased (fBm)" << endl;
    screenquad.updateOctaves(false);
    reDrawNoise();
    break;
    case 'K':
        cout << "Offset increased (fBm)" << endl;
        screenquad.updateOffset(true);
        reDrawNoise();
         break;
    case 'J':
    cout << "Offset decreased (fBm)" << endl;
    screenquad.updateOffset(false);
    reDrawNoise();
    break;
    case 'W':
        cout<<"Moving forward"<<endl;
        moveView(0);
        break;

    case 'S':
        cout<<"Moving backward"<<endl;
        moveView(1);
        break;

    case 'A':
        cout<<"Moving left"<<endl;
        moveView(3);
        break;
    case 'D':
        cout<<"Moving right"<<endl;
        moveView(2);
        break;
    case 'Q':
        cout<<"Looking Up"<<endl;
        moveView(4);
        break;
    case 'E':
        cout<<"Looking Down"<<endl;
        moveView(5);
        break;
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
                                          "Trackball", NULL, NULL);
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

    // set the mouse press and position callback
    glfwSetMouseButtonCallback(window, MouseButton);
    glfwSetCursorPosCallback(window, MousePos);

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
