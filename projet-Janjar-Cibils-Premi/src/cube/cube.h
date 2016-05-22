#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

class Cube {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID
        GLuint num_indices_;                    // number of vertices to render
        GLuint MVP_id_;                         // model, view, proj matrix ID
        GLuint texture_sky_id;
        GLuint cubeTexture;
        GLuint cubemapTexture;
        GLuint textureID;

    public:
        void loadTexture() {

            vector<const GLchar*> faces;
            faces.push_back("cloudtop_rt.tga");
            faces.push_back("cloudtop_lf.tga");
            faces.push_back("cloudtop_up.tga");
            faces.push_back("cloudtop_dn.tga");
            faces.push_back("cloudtop_bk.tga");
            faces.push_back("cloudtop_ft.tga");

            int width,height;
            int nb_component;
            unsigned char* image;


            glGenTextures(1, &textureID);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
            for(GLuint i = 0; i < faces.size(); i++)
            {
                image = stbi_load(faces[i], &width,
                                  &height, &nb_component, 0);
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
                );
                stbi_image_free(image);
            }


            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }

        void Init() {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("cube_vshader.glsl",
                                                  "cube_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);


            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // Position buffer
            const GLfloat position[] = {-6.0f, -6.0f,  6.0f, // left, bottom, front
                                        6.0f, -6.0f,  6.0f,  // right, bottom, front
                                        6.0f,  6.0f,  6.0f,  // right, top, front
                                        -6.0f,  6.0f,  6.0f, // left, top, front
                                        -6.0f, -6.0f, -6.0f, // left, bottom, back
                                        6.0f, -6.0f, -6.0f,  // right, bottom, back
                                        6.0f,  6.0f, -6.0f,  // right, top, back
                                        -6.0f,6.0f, -6.0f};// left, top, back

            glGenBuffers(1, &vertex_buffer_object_position_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

            // position shader attribute
            // fetch attribute ID for vertex positions
            GLuint loc_position = glGetAttribLocation(program_id_, "position");
            glEnableVertexAttribArray(loc_position); // Enable it
            glVertexAttribPointer(loc_position, 3, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);

            // index buffer
            const GLuint index[] = {0, 1, 2,  // front face triangle 1
                                    0, 2, 3,  // front face triangle 2
                                    1, 5, 6,  // right face triangle 1
                                    1, 6, 2,  // right face triangle 2
                                    5, 4, 7,  // back face triangle 1
                                    5, 7, 6,  // back face triangle 2
                                    4, 0, 3,  // left face triangle 1
                                    4, 3, 7,  // left face triangle 2
                                    3, 2, 6,  // top face triangle 1
                                    3, 6, 7,  // top face triangle 2
                                    1, 0, 4,  // bottom face triangle 1
                                    1, 4, 5}; // bottom face triangle 2

            num_indices_ = sizeof(index) / sizeof(GLuint);

            glGenBuffers(1, &vertex_buffer_object_index_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

            // other uniforms
            MVP_id_ = glGetUniformLocation(program_id_, "MVP");

            // to avoid the current object being polluted
            glBindVertexArray(0);

            loadTexture();
            glUseProgram(0);
        }



        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_position_);
            glDeleteBuffers(1, &vertex_buffer_object_index_);
            glDeleteProgram(program_id_);
            glDeleteTextures(1,&texture_sky_id);
            glDeleteVertexArrays(1, &vertex_array_id_);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {

            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);
            //glDepthMask(GL_FALSE);// Remember to turn depth writing off


            glm::mat4 MVP = projection*view*model;
            glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

                    glActiveTexture(GL_TEXTURE0);
                    glUniform1i(glGetUniformLocation(program_id_, "skybox"), 0);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
                    glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);
                  //  glDrawArrays(GL_TRIANGLES, 0, 36);
                    glBindVertexArray(0);
                    //glDepthMask(GL_TRUE);

            // setup MVP


            // draw
           // glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
