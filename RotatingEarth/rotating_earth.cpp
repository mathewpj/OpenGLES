/************************************************************** 
 *  
 *  OpenGLES 3.0 Program to map a earth texture to a sphere
 *  & rotate the earth for every key press.
 * 
 *  Running OpenglEs on desktop requires an emulator.
 *  You could use an emulator like Mali OpenGL ES 
 *  emulator however the minimum requirements are 
 *  - OpenGL 3.2 – when OpenGL ES 2.0 contexts are used
 *  - OpenGL 3.3 – when OpenGL ES 3.0 contexts are used
 *  - OpenGL 4.3 – when OpenGL ES 3.1 or 3.2 contexts are used
 *  and my system only supports OpenGL 3.0. 
 *  Hence using GLFW library. 
 *
 *  Ensure to enable GL_DEPTH_TEST * set glDepthFunc(GL_LEQUAL)    
 * 
 *  sudo apt-get install libglfw3-dev libgles2-mesa-dev
 *  gcc rotating_earth.cpp sphere_gen.cpp transform.cpp 
 *  load_shader.cpp readBMP.cpp -g -I . -lGLESv2 -lglfw -lm 
 *  -o RotatingEarth  
 *
 *  Author : Mathew P Joseph
 *  email  : mathew.p.joseph@gmail.com
 *
 *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <matrix.h>
#include "readBMP.h"

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

extern void esMatrixLoadIdentity ( ESMatrix *result );
extern void esMatrixMultiply ( ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB );
extern void esMatrixMultiply ( ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB );
extern void esTranslate ( ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz );
extern void esRotate ( ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
extern void esFrustum ( ESMatrix *result, float left, float right, float bottom, 
            float top, float nearZ, float farZ );
void esOrtho ( ESMatrix *result, float left, float right, float bottom, 
	float top, float nearZ, float farZ );
extern GLint common_get_shader_program(const char *vertex_shader_source, 
	    const char *fragment_shader_source);
int  esGenSphere ( int numSlices, float radius, GLfloat **vertices, GLfloat **normals,
                             GLfloat **texCoords, GLuint **indices );

static const GLuint WIDTH  = 500;
static const GLuint HEIGHT = 500;

    // Making these global variables 

    GLuint shader_program;
    GLint numIndices;
    GLFWwindow* window;
    
    // Vertex data
    GLfloat  *vertices;
    GLfloat  *texcoords;
    GLuint   *indices;
   
    // Matrices for perspective, modelview & modelview-perpective	
    ESMatrix perspective;
    ESMatrix ortho;
    ESMatrix modelview;
    ESMatrix mvpMatrix;
    
    // Uniform Model View Projection location
    GLint  mvpLoc;
    GLint samplerLoc;

    // Shader "IN" locations	
    GLint ver, tex;
   
    GLfloat  aspect;
    GLfloat  angle;

    GLuint textureId;   

static const GLchar* vertex_shader_source =
      "#version 300 es                            \n"
      "uniform mat4 u_mvpMatrix;                  \n"
      "layout(location = 0) in vec4 a_position;   \n"
      "layout(location = 1) in vec2 a_texCoord;   \n"	
      "out vec2 v_texCoord;                       \n"
      "void main()                                \n"
      "{                                          \n"
      "   gl_Position = a_position*u_mvpMatrix;	  \n"
      "   v_texCoord = a_texCoord;                \n"
      "}                                          \n";
static const GLchar* fragment_shader_source =
      "#version 300 es				  \n"
      "precision mediump float;			  \n"
      "in vec4 v_color;				  \n"
      "in vec2 v_texCoord;                        \n"
      "layout(location = 0) out vec4 outColor;	  \n"	
      "uniform sampler2D s_texture;               \n"
      "void main() {				  \n"
      "outColor = texture(s_texture, v_texCoord );\n"
      "}					  \n";
struct Image image;

GLuint CreateSimpleTexture2D( )
{
   // Texture object handle
   GLuint textureId;
   char BMPfile[] = "earth.bmp";
   ImageLoad(BMPfile, &image);
   char *pixels = image.data;

   // Use tightly packed data
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

   // Generate a texture object
   glGenTextures ( 1, &textureId );

   // Bind the texture object
   glBindTexture ( GL_TEXTURE_2D, textureId );

   // Load the texture
   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, image.sizeX, image.sizeY, 
                  0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

   // Set the filtering mode
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   // Set the texture wrap around mode
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   return textureId;

}

  //!GLFW keyboard callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static int entry = 0;
    // key_callback somehow gets called twice for every
    // key press. So limiting it to one...	 
    if(entry%2 == 0){
    // Generate a model view matrix to rotate/translate the sphere
    esMatrixLoadIdentity ( &modelview );
		// Rotate the sphere
    		angle += 1.0f;
    		esRotate (&modelview, angle, 0.0, 1.0, 0.0 );

    		// Compute the final MVP by multiplying the
    		// modevleiw and orthographic matrices together
    		esMatrixMultiply ( &mvpMatrix, &modelview, &ortho );
    		// Load the MVP matrix
    		glUniformMatrix4fv ( mvpLoc, 1, GL_FALSE, ( GLfloat * ) &mvpMatrix.m[0][0] );
     	}
     entry++;		
} 

int main(int argc, char* argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, NULL, NULL);
    if(window==NULL){	
	printf("glfwCreateWindow Error \n");
	return -1;
    }		
    glfwMakeContextCurrent(window);
    //Keyboard Callback
    glfwSetKeyCallback(window, key_callback);

    // Commenting out GL_DEPTH_TEST seems to 
    // have a +ve effect on rendering the sphere when 
    // its rotated but adds new artifacts
    //glEnable ( GL_DEPTH_TEST );
    //glDepthFunc(GL_LEQUAL);  
    printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
    printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );

    // Generate an orthographic projection matrix for the shadow map rendering
    esMatrixLoadIdentity (&ortho);
    esOrtho ( &ortho, -2, 2, -2, 2, -2, 2 );

    // Generate a model view matrix to rotate/translate the sphere
    esMatrixLoadIdentity ( &modelview );

    // Rotate the sphere
    angle = 30.0f;	
    esRotate ( &modelview, angle, 0.0, 1.0, 0.0 );

    esMatrixMultiply ( &mvpMatrix, &modelview, &ortho );

    shader_program = common_get_shader_program(vertex_shader_source, fragment_shader_source);
    textureId = CreateSimpleTexture2D ();   
 
    // Get the uniform,"in" & sampler variable locations
    ver = glGetAttribLocation(shader_program, "a_position");
    tex = glGetAttribLocation(shader_program, "a_texCoord");
    mvpLoc = glGetUniformLocation ( shader_program, "u_mvpMatrix" );
    samplerLoc = glGetUniformLocation ( shader_program, "s_texture" );

    glViewport(0, 0, WIDTH, HEIGHT);
    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
 
    // Enable the shaders. This step needs to be done before 
    // loading the uniform variables & passing "in" variables 
    // to shader
    glUseProgram(shader_program);


    numIndices = esGenSphere ( 300, 1.5f, &vertices, NULL,
                              &texcoords, &indices );

    glVertexAttribPointer(ver, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(ver);
   
    // Load the texture coordinate
    glVertexAttribPointer (tex, 2, GL_FLOAT, GL_FALSE, 0, texcoords); 
    glEnableVertexAttribArray (tex); 
    // Load the MVP matrix
    glUniformMatrix4fv ( mvpLoc, 1, GL_FALSE, ( GLfloat * ) &mvpMatrix.m[0][0] );

    // Bind the texture
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, textureId );
    
    // Set the sampler texture unit to 0
    glUniform1i ( samplerLoc, 0 );
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements ( GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices );
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}


