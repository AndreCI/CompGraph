#pragma once
#include "icg_helper.h"

class FrameBuffer {

    private:
        int width_;
        int _;
        GLuint depth_render_buffer_id_;

        GLuint _texture_id_;

    public:
        GLuint framebuffer_object_id_;
        // warning: overrides viewport!!
        void Bind() {
            glViewport(0, 0, width_, _);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
            const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, };
            glDrawBuffers(1 /*length of buffers[]*/, buffers);
        }

        void Unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        int Init(int image_width, int image_,
                                  bool use_interpolation = false) {
            this->width_ = image_width;
            this->_ = image_;

            // create color attachment
            {
                glGenTextures(1, &_texture_id_);
                glBindTexture(GL_TEXTURE_2D, _texture_id_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                if(use_interpolation){
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                } else {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                }


               }

                // create render buffer (for depth channel)
                {
                    glGenRenderbuffers(1, &depth_render_buffer_id_);
                    glBindRenderbuffer(GL_RENDERBUFFER, depth_render_buffer_id_);
                    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width_, _);
                    glBindRenderbuffer(GL_RENDERBUFFER, 0);
                }


            {

                // create texture for the color attachment
                // see Table.2 on
                // khronos.org/opengles/sdk/docs/man3/docbook4/xhtml/glTexImage2D.xml
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width_, _, 0,
                             GL_RGB, GL_FLOAT, NULL);
                // how to load from buffer
            }




            // tie it all together
            {
                glGenFramebuffers(1, &framebuffer_object_id_);
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                                       GL_COLOR_ATTACHMENT0 /*location = 0*/,
                                       GL_TEXTURE_2D, _texture_id_,
                                       0 /*level*/);

                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                          GL_RENDERBUFFER, depth_render_buffer_id_);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
                    GL_FRAMEBUFFER_COMPLETE) {
                    cerr << "!!!ERROR: Framebuffer not OK :(" << endl;
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0); // avoid pollution
            }

            return _texture_id_;
        }
        void ClearContent() {
            glViewport(0, 0, width_, _);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glClearColor(1.0, 1.0, 1.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Cleanup() {
            glDeleteTextures(1, &_texture_id_);
            glDeleteRenderbuffers(1, &depth_render_buffer_id_);
            glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
            glDeleteFramebuffers(1, &framebuffer_object_id_);
        }
};
