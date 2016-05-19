#pragma once
#include "icg_helper.h"
#include <stdlib.h>

class ScreenQuad {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_;             // texture ID
        GLuint texture_velocity_id_;    // texture velocity ID
        GLuint h_id;
        GLuint lacunarity_id;
        GLuint octaves_id;
        GLuint offset_id;

        float screenquad_width_;
        float screenquad_height_;
        float h_value;
        float lacunarity_value;
        int octaves_value;
        float offset_value;

    public:
        void Init(float screenquad_width, float screenquad_height,
                  GLuint texture) {



            // set screenquad size
            this->screenquad_width_ = screenquad_width;
            this->screenquad_height_ = screenquad_height;

            // compile the shaders
            program_id_ = icg_helper::LoadShaders("screenquad_vshader.glsl",
                                                  "screenquad_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                const GLfloat vertex_point[] = { /*V1*/ -2.0f, -2.0f, 0.0f,
                                                 /*V2*/ +2.0f, -2.0f, 0.0f,
                                                 /*V3*/ -2.0f, +2.0f, 0.0f,
                                                 /*V4*/ +2.0f, +2.0f, 0.0f};
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point),
                             vertex_point, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // texture coordinates
            {
                const GLfloat vertex_texture_coordinates[] = { /*V1*/ 0.0f, 0.0f,
                                                               /*V2*/ 4.0f, 0.0f,
                                                               /*V3*/ 0.0f, 4.0f,
                                                               /*V4*/ 4.0f, 4.0f};

                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates),
                             vertex_texture_coordinates, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_,
                                                                     "vtexcoord");
                glEnableVertexAttribArray(vertex_texture_coord_id);
                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE,
                                      ZERO_BUFFER_OFFSET);

                h_id= glGetUniformLocation(program_id_,"h_fBm");
                lacunarity_id = glGetUniformLocation(program_id_,"lacunarity_fBm");
                octaves_id = glGetUniformLocation(program_id_,"octaves_fBm");
                offset_id = glGetUniformLocation(program_id_,"offset_fBm");

                h_value=1.2;
                lacunarity_value = 3.7;
                octaves_value = 12;
                offset_value = 0.32;
                glUniform1f(h_id,h_value);
                glUniform1f(lacunarity_id,lacunarity_value);
                glUniform1i(octaves_id,octaves_value);
                glUniform1f(offset_id,offset_value);

            }

        }

        void updateH(bool increase){
            if(increase){
                h_value+=0.05;
            }else{
                if(h_value>0.05){
                h_value-=0.05;
                }
            }
        }

        void updateLacunarity(bool increase){
            if(increase){
                lacunarity_value+=0.05;
            }else{
                if(lacunarity_value>0.05){
                lacunarity_value-=0.05;
                }
            }
        }
        void updateOffset(bool increase){
            if(increase){
                offset_value+=0.01;
            }else{
                if(offset_value>0.01){
               offset_value-=0.01;
                }
            }
        }
        void updateOctaves(bool increase){
            if(increase){
                octaves_value++;
            }else{
                if(octaves_value>1){
                octaves_value--;
                }
            }
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteTextures(1, &texture_id_);
        }

        void UpdateSize(int screenquad_width, int screenquad_height) {
            this->screenquad_width_ = screenquad_width;
            this->screenquad_height_ = screenquad_height;
        }

        void Draw() {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);


            glUniform1f(h_id,h_value);
            // draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);



            glBindVertexArray(0);
            glUseProgram(0);
        }

        void DrawNoise(){
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            glUniform1f(h_id,h_value);
            glUniform1f(lacunarity_id,lacunarity_value);
            glUniform1i(octaves_id,octaves_value);
            glUniform1f(offset_id,offset_value);
            // draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
