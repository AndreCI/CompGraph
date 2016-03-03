#pragma once
#include "icg_helper.h"


class Triangle {

    private:
        GLuint vertex_array_id;        // vertex array object
        GLuint program_id;             // GLSL shader program ID
        GLuint vertex_buffer;   // memory buffer
        glm::mat4 M = glm::mat4(1.0f);

    public:

        void Init() {
            // LoadShaders                  <--- compile the shaders
            // glUseProgram                 <--- bind the shader (for attributes)
            //---
            // glGenVertexArrays            <--- create vertex array
            // glBindVertexArray            <--- and bind it
            //---
            // glGenBuffers                 <--- generate buffer pointer
            // glBindBuffers                <--- make it current
            // glBufferData                 <--- tell it where to find data
            //---
            // glGetAttribLocation          <--- fetch attribute ID
            // glEnableVertexAttribArray    <--- make it the current
            // glVertexAttribPointer        <--- specify layout of attribute
            //---
            // stbi_load                    <--- load image from file
            // glGenTextures                <--- create texture buffer
            // glBindTexture                <--- and bind it
            // glTexImage2D                 <--- fill the texture with image
            // stbi_image_free              <--- free the image in cpu memory
            //---
            // glGetUniformLocation         <--- fetch ID from shader
            // glUniform*(...)              <--- set the values of a uniform
            //--- To avoid resource pollution, unload resources
            // glUseProgram(0)              <--- unbind program (safety!)
            // glBindVertexArray(0)         <--- unbind array (safety!)


            // compile the shaders
            const GLfloat vpoint[] = {-1.0f, -1.0f, 0.0f,
                                      1.0f, -1.0f, 0.0f,
                                      0.0f,  1.0f, 0.0f,};
            program_id = icg_helper::LoadShaders("triangle_vshader.glsl",
                                                        "triangle_fshader.glsl");
            if(!program_id) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id);

            // Vertex Array
            glGenVertexArrays(1,&vertex_array_id);
            glBindVertexArray(vertex_array_id);

            // Vertex Buffer
            glGenBuffers(1, &vertex_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

            // position attribute, fetch Attribute ID for Vertex Positions
            GLuint position = glGetAttribLocation(program_id, "vpoint");
            glEnableVertexAttribArray(position); // enable it
            glVertexAttribPointer(position, 3, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);


            GLuint M_id = glGetUniformLocation(program_id, "M");
            glUniformMatrix4fv(M_id, 1, GL_FALSE, glm::value_ptr(M));
        }

        void createMMatrix(double alpha, double sx, double sy, double tx, double ty){
            glm::mat4 R = glm::mat4(0);
            glm::mat4 S = glm::mat4(0);
            glm::mat4 T = glm::mat4(0);
            R[0][0] = cos(alpha);
            R[0][1] = sin(alpha);
            R[1][0] = -sin(alpha);
            R[1][1] = cos(alpha);
            R[2][2] = 1;
            R[3][3] = 1;
            S[0][0] = sx;
            S[1][1] = sy;
            S[2][2] = 1;
            S[3][3] = 1;
            T[0][0] = 1;
            T[1][1] = 1;
            T[2][2] = 1;
            T[3][3] = 1;
            T[3][0] = tx;
            T[3][1] = ty;
            M = T*S*R;
        }

        void Cleanup() {
             //glDelete*();
        }
        
        void Draw() {
             glUseProgram(program_id);
             glBindVertexArray(vertex_array_id);
            //--- if there is a texture :
            // glActiveTexture          <--- enable certain tex-unit
            // glBindTexture            <--- bind it to CPU memory
            //--- if there a "changing" value to pass to the shader :
            // glUniform*(...)          <--- set the values of a uniform
            //---
            // glDraw*(...)             <--- draws the buffers
            //---
             glDrawArrays(GL_TRIANGLES, 0, 3);
             glBindVertexArray(0);
             glUseProgram(0);

        }
};
