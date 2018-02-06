/************************************************************** 
 *
 *  Opengl ES 3.0 2D Texture mapping
 *  Reads a texture from a bmp file  
 *
 *  sudo apt-get install libglfw3-dev libgles2-mesa-dev
 *  gcc texture.cpp readBMP.cpp -lGLESv2 -lglfw -o 2Dtexture
 *
 *  Appropriator : mathew.p.joseph@gmail.com
 *
 *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "readBMP.h"

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

static const GLuint WIDTH = 800;
static const GLuint HEIGHT = 600;
static const GLchar* vertex_shader_source =
      "#version 300 es                            \n"
      "layout(location = 0) in vec4 a_position;   \n"
      "layout(location = 1) in vec2 a_texCoord;   \n"
      "out vec2 v_texCoord;                       \n"
      "void main()                                \n"
      "{                                          \n"
      "   gl_Position = a_position;               \n"
      "   v_texCoord = a_texCoord;                \n"
      "}                                          \n";
static const GLchar* fragment_shader_source =
      "#version 300 es                                     \n"
      "precision mediump float;                            \n"
      "in vec2 v_texCoord;                                 \n"
      "layout(location = 0) out vec4 outColor;             \n"
      "uniform sampler2D s_texture;                        \n"
      "void main()                                         \n"
      "{                                                   \n"
      "  outColor = texture( s_texture, v_texCoord );      \n"
      "}                                                   \n";
static const GLfloat vVertices[] = {
                            -0.5f, -0.5f, 0.0f,  // Position 0
                             0.0f,  0.0f,        // TexCoord 0 
                             0.5f, -0.5f, 0.0f,  // Position 1
                             0.0f,  1.0f,        // TexCoord 1
                             0.5f,  0.5f, 0.0f,  // Position 2
                             1.0f,  1.0f,        // TexCoord 2
                            -0.5f,  0.5f, 0.0f,  // Position 3
                             1.0f,  0.0f         // TexCoord 3
                         };
GLushort indices[] = { 1, 2, 0, 0, 2, 3 };

// Sampler location
GLint samplerLoc;
// Texture handle
GLuint textureId;

struct Image image;

//
// Create a simple 2x2 texture image with four different colors
//
GLuint CreateSimpleTexture2D( )
{
   // Texture object handle
   GLuint textureId;
   char BMPfile[] = "crate.bmp";
   ImageLoad(BMPfile, &image);
   char *pixels = image.data;

   // Use tightly packed data
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

   // Generate a texture object
   glGenTextures ( 1, &textureId );

   // Bind the texture object
   glBindTexture ( GL_TEXTURE_2D, textureId );

   // Load the texture
   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, image.sizeX, image.sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

   // Set the filtering mode
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

   return textureId;

}
        
GLint common_get_shader_program(const char *vertex_shader_source, const char *fragment_shader_source) {
    enum Consts {INFOLOG_LEN = 512};
    GLchar infoLog[INFOLOG_LEN];
    GLint fragment_shader;
    GLint shader_program;
    GLint success;
    GLint vertex_shader;

    /* Vertex shader */
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    /* Fragment shader */
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    /* Link shaders */
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    /* Get the sampler location */
    samplerLoc = glGetUniformLocation ( shader_program, "s_texture" );

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader_program;
}

int main(void) {
    GLuint shader_program, vbo;
    GLint pos;
    GLFWwindow* window;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, NULL, NULL);
    glfwMakeContextCurrent(window);

    printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
    printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );

    shader_program = common_get_shader_program(vertex_shader_source, fragment_shader_source);
    textureId = CreateSimpleTexture2D ();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, WIDTH, HEIGHT);

    // Clear the color buffer - Added
    glClear ( GL_COLOR_BUFFER_BIT );

    // Load the vertex position
    glVertexAttribPointer ( 0, 3, GL_FLOAT,
                           GL_FALSE, 5 * sizeof ( GLfloat ), vVertices );
    // Load the texture coordinate
    glVertexAttribPointer ( 1, 2, GL_FLOAT,
                           GL_FALSE, 5 * sizeof ( GLfloat ), &vVertices[3] );

    glEnableVertexAttribArray ( 0 );
    glEnableVertexAttribArray ( 1 );

    // Bind the texture
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, textureId );

    // Set the sampler texture unit to 0
    glUniform1i ( samplerLoc, 0 );

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);
        glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );	
        glfwSwapBuffers(window);
    }
    glDeleteBuffers(1, &vbo);
    glfwTerminate();
    return EXIT_SUCCESS;
}
