#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

class Grid {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID
        GLuint texture_id_;                     // texture ID
        GLuint num_indices_;                    // number of vertices to render
        GLuint MVP_id_;                         // model, view, proj matrix ID
        GLuint perlin_tex_id_;                  // texture perlin ID
        GLuint river_tex_id_;
        GLuint vertex_buffer_object_;           // memory buffer

        GLuint perlin_tex_id;
        GLuint river_tex_id;
        GLuint texture_sand_id;
        GLuint texture_rock_id;
        GLuint texture_grass_id;
        GLuint texture_snow_id;
        GLuint texture_water_id;

    public:

        void Init(GLuint perlin_tex = -1, GLuint river_tex = -1, int nbr_triangle = 256, float *riversPoints = NULL, int riverPointsSize = 0) {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("grid_vshader.glsl",
                                                  "grid_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates and indices
            {
                std::vector<GLfloat> vertices;
                std::vector<GLuint> indices;
                int nbretriangles = nbr_triangle;

              glGenBuffers(1, &vertex_buffer_object_);
              glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);

              GLfloat step = 4.0f/(nbretriangles-1);

        for( int i=0; i<=nbretriangles;++i ){
            for(int j = 0;j<=nbretriangles;++j){
            GLfloat x =i*(step)-1.0;
            GLfloat y =j*(step)-1.0;
			vertices.push_back(x); 
            vertices.push_back(y);
		}	
    }
            for(int i = 0 ; i<nbretriangles-1 ;++i){
                for(int j = 0; j<nbretriangles-1; ++j){
                    int sommet1 = ((nbretriangles+1)*i)+j;
                    int sommet3 = sommet1+nbretriangles+2;
                    int sommet2 = sommet1+nbretriangles+1;
                    indices.push_back(sommet1);
                    indices.push_back(sommet2);
                    indices.push_back(sommet3);
                    int sommet4 = ((nbretriangles+1)*i)+j+1;
                    indices.push_back(sommet1);
                    indices.push_back(sommet4);
                    indices.push_back(sommet3);
                }
			}
                num_indices_ = indices.size();

                // position buffer
                glGenBuffers(1, &vertex_buffer_object_position_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                             &vertices[0], GL_STATIC_DRAW);

                // vertex indices
                glGenBuffers(1, &vertex_buffer_object_index_);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                             &indices[0], GL_STATIC_DRAW);

                // position shader attribute
                GLuint loc_position = glGetAttribLocation(program_id_, "position");
                glEnableVertexAttribArray(loc_position);
                glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE,
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
                        }
            {

                int width;
                int height;
                int nb_component;
                // set stb_image to have the same coordinates as OpenGL

                //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                string filename ="sand.jpg";
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image_sand = stbi_load(filename.c_str(), &width,
                                                 &height, &nb_component, 0);

                if(image_sand == nullptr) {
                    throw(string("Failed to load texture"));
                }

                glGenTextures(1, &texture_sand_id);
                glBindTexture(GL_TEXTURE_2D, texture_sand_id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image_sand);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image_sand);
                }
                GLuint texture_sand = glGetUniformLocation(program_id_,"texture_sand");
                glUniform1i(texture_sand,2 /*GL_TEXTURE2*/);


                //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                filename ="rock.jpg";
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image_rock = stbi_load(filename.c_str(), &width,
                                                 &height, &nb_component, 0);

                if(image_rock == nullptr) {
                    throw(string("Failed to load texture"));
                }
                glGenTextures(1, &texture_rock_id);
                glBindTexture(GL_TEXTURE_2D, texture_rock_id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image_rock);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image_rock);
                }
                GLuint texture_rock = glGetUniformLocation(program_id_,"texture_rock");
                glUniform1i(texture_rock,3 /*GL_TEXTURE3*/);

                //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

                filename ="grass.jpg";
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image_grass = stbi_load(filename.c_str(), &width,
                                                 &height, &nb_component, 0);

                if(image_grass == nullptr) {
                    throw(string("Failed to load texture"));
                }
                glGenTextures(1,&texture_grass_id);
                glBindTexture(GL_TEXTURE_2D, texture_grass_id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image_grass);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image_grass);
                }

                GLuint texture_grass = glGetUniformLocation(program_id_,"texture_grass");
                glUniform1i(texture_grass,4 /*GL_TEXTURE4*/);

                //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

                filename ="snow.jpg";
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image_snow = stbi_load(filename.c_str(), &width,
                                                 &height, &nb_component, 0);

                if(image_snow == nullptr) {
                    throw(string("Failed to load texture"));
                }

                glGenTextures(1,&texture_snow_id);
                glBindTexture(GL_TEXTURE_2D, texture_snow_id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image_snow);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image_snow);
                }
               // glBindTexture(GL_TEXTURE_2D, 0);
                 //               stbi_image_free(image_snow);

               GLuint texture_snow = glGetUniformLocation(program_id_,"texture_snow");
                glUniform1i(texture_snow,5 /*GL_TEXTURE5*/);

                //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

                filename ="water.jpg";
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image_water = stbi_load(filename.c_str(), &width,
                                                 &height, &nb_component, 0);

                if(image_water == nullptr) {
                    throw(string("Failed to load texture"));
                }

                glGenTextures(1,&texture_water_id);
                glBindTexture(GL_TEXTURE_2D, texture_water_id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image_water);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image_water);
                }

               GLuint texture_water = glGetUniformLocation(program_id_,"texture_water");
                glUniform1i(texture_water,6 /*GL_TEXTURE6*/);

                //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><
                perlin_tex_id_ = (perlin_tex==-1)? texture_id_ : perlin_tex;

              //  glUseProgram(program_id_);

                // texture uniforms
                perlin_tex_id = glGetUniformLocation(program_id_, "heightTex");
                glUniform1i(perlin_tex_id, 1 /*GL_TEXTURE1*/);

                river_tex_id_ = river_tex;
                glUseProgram(program_id_);

                // texture uniforms
                river_tex_id = glGetUniformLocation(program_id_, "riverTex");
                glUniform1i(river_tex_id, 7 /*GL_TEXTURE7*/);

                //<<<<<<<<

            }

            // other uniforms
            MVP_id_ = glGetUniformLocation(program_id_, "MVP");
            GLuint riverpoints = glGetUniformLocation(program_id_, "riverPoints");
            glUniform2fv(riverpoints,riverPointsSize,(GLfloat*)riversPoints);
            GLuint riverpointssize = glGetUniformLocation(program_id_,"riverPointsSize");
            glUniform1i(riverpointssize,riverPointsSize);
            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }



        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_position_);
            glDeleteBuffers(1, &vertex_buffer_object_index_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteProgram(program_id_);
            glDeleteTextures(1, &texture_id_);
            glDeleteTextures(1, &perlin_tex_id_);
            glDeleteTextures(1, &texture_sand_id);
            glDeleteTextures(1, &texture_grass_id);
            glDeleteTextures(1, &texture_rock_id);
            glDeleteTextures(1, &texture_snow_id);
            glDeleteTextures(1, &texture_water_id);
        }


        void Draw(float time, const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                   const glm::mat4 &projection = IDENTITY_MATRIX) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, perlin_tex_id_);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, texture_sand_id);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, texture_rock_id);

            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, texture_grass_id);

            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, texture_snow_id);


            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, texture_water_id);

            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, river_tex_id_);

            // setup MVP

            glm::mat4 MVP = projection*view*model;
            glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));
            glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }

};






