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
        GLuint vertex_buffer_object_;           // memory buffer

        GLuint perlin_tex_id;

    public:
        void Init(GLuint perlin_tex = -1) {
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
                // TODO 5: make a triangle grid with dimension 100x100.
                // always two subsequent entries in 'vertices' form a 2D vertex position.

                int nbretriangles = 512;


              glGenBuffers(1, &vertex_buffer_object_);
              glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);

        GLfloat step = 2.0f/(nbretriangles-1);

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

            //>>>>>>>>
            // texture coordinates
                        {
                            const GLfloat vertex_texture_coordinates[] = { /*V1*/ 0.0f, 0.0f,
                                                                           /*V2*/ 1.0f, 0.0f,
                                                                           /*V3*/ 0.0f, 1.0f,
                                                                           /*V4*/ 1.0f, 1.0f};

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
            //<<<<<<<


            {
                int width;
                int height;
                int nb_component;
                string filename = "grid_texture.tga";
                // set stb_image to have the same coordinates as OpenGL
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image = stbi_load(filename.c_str(), &width,
                                                 &height, &nb_component, 0);

                if(image == nullptr) {
                    throw(string("Failed to load texture"));
                }

                glGenTextures(1, &texture_id_);
                glBindTexture(GL_TEXTURE_2D, texture_id_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image);
                }
//>>>>>>>
                perlin_tex_id_ = (perlin_tex==-1)? texture_id_ : perlin_tex;

                // texture uniforms
                GLuint tex_id = glGetUniformLocation(program_id_, "tex");
                glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
                perlin_tex_id = glGetUniformLocation(program_id_, "heightTex");
                glUniform1i(perlin_tex_id, 1 /*GL_TEXTURE1*/);
//<<<<<<<<

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(image);
            }

            // other uniforms
            MVP_id_ = glGetUniformLocation(program_id_, "MVP");

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
        }


        void Draw(float time, const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, perlin_tex_id_);

            // setup MVP
            glm::mat4 MVP = projection*view*model;
            glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

            // pass the current time stamp to the shader.
            glUniform1f(glGetUniformLocation(program_id_, "time"), time);

            // draw
            // TODO 5: for debugging it can be helpful to draw only the wireframe.
            // You can do that by uncommenting the next line.
           //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            // TODO 5: depending on how you set up your vertex index buffer, you
            // might have to change GL_TRIANGLE_STRIP to GL_TRIANGLES.
            glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }

};






