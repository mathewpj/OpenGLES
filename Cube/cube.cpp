/************************************************************** 
 *  
 *  OpenGLES 3.0 Program to render a cube.
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
 *  sudo apt-get install libglfw3-dev libgles2-mesa-dev
 *  gcc cube.cpp cube_gen.cpp transform.cpp load_shader.cpp 
 *  -I . -lGLESv2 -lglfw -lm -o 3DCube
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

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

extern void esMatrixLoadIdentity ( ESMatrix *result );
extern void esMatrixMultiply ( ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB );
extern void esMatrixMultiply ( ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB );
extern void esTranslate ( ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz );
extern void esRotate ( ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
extern void esFrustum ( ESMatrix *result, float left, float right, float bottom, 
            float top, float nearZ, float farZ );
extern void esPerspective ( ESMatrix *result, float fovy, float aspect, float nearZ, 
	    float farZ );
extern GLint common_get_shader_program(const char *vertex_shader_source, 
	    const char *fragment_shader_source);
extern int esGenCube ( float scale, GLfloat **vertices, GLfloat **normals,
                           GLfloat **texCoords, GLuint **indices );

static const GLuint WIDTH  = 800;
static const GLuint HEIGHT = 600;

static const GLchar* vertex_shader_source =
      "#version 300 es                            \n"
      "uniform mat4 u_mvpMatrix;                  \n"
      "layout(location = 0) in vec4 a_position;   \n"
      "layout(location = 1) in vec4 a_color;	  \n"
      "out vec4 v_color;			  \n"
      "void main()                                \n"
      "{                                          \n"
      "   v_color = a_color;    		   \n"
      "   gl_Position = a_position*u_mvpMatrix;	  \n"
      "}                                          \n";
static const GLchar* fragment_shader_source =
    "#version 300 es				  \n"
    "precision mediump float;			  \n"
    "in vec4 v_color;				  \n"
    "layout(location = 0) out vec4 fragColor;	  \n"	
    "void main() {				  \n"
    "   fragColor = v_color;			  \n"
    "}						  \n";

int main(int argc, char* argv[]) {
    GLuint shader_program;
    GLint numIndices;
    GLFWwindow* window;
    
    // Vertex data
    GLfloat  *vertices;
    GLuint   *indices;
   
    // Matrices for perspective, modelview & modelview-perpective	
    ESMatrix perspective;
    ESMatrix modelview;
    ESMatrix mvpMatrix;
    
    // Uniform Model View Projection location
    GLint  mvpLoc;

    // Shader "IN" locations	
    GLint ver, col;
    
    GLfloat  aspect;
    GLfloat  angle;
   

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, NULL, NULL);
    if(window==NULL){	
	printf("glfwCreateWindow Error \n");
	return -1;
    }		
    glfwMakeContextCurrent(window);

    printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
    printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );

    // Compute the window aspect ratio
    aspect = ( GLfloat )WIDTH/( GLfloat )HEIGHT;
	
    // Generate a perspective matrix with a 60 degree FOV
    esMatrixLoadIdentity ( &perspective );
    esPerspective ( &perspective, 60.0f, aspect, 1.0f, 20.0f );

    // Generate a model view matrix to rotate/translate the cube
    esMatrixLoadIdentity ( &modelview );

    // Translate away from the viewer
    esTranslate ( &modelview, 0.0, 0.0, -2.0 );

    // Rotate the cube
    angle = 45.0f;	
    esRotate ( &modelview, angle, 1.0, 1.0, 1.0 );

    // Compute the final MVP by multiplying the
    // modevleiw and perspective matrices together
    esMatrixMultiply ( &mvpMatrix, &modelview, &perspective );

    shader_program = common_get_shader_program(vertex_shader_source, fragment_shader_source);
    
    // Get the uniform & "in" variable locations
    ver = glGetAttribLocation(shader_program, "a_position");
    col = glGetAttribLocation(shader_program, "a_color");
    mvpLoc = glGetUniformLocation ( shader_program, "u_mvpMatrix" );

    glViewport(0, 0, WIDTH, HEIGHT);
    
    // Enable the shaders. This step needs to be done before 
    // loading the uniform variables & passing "in" variables 
    // to shader
    glUseProgram(shader_program);

    numIndices = esGenCube ( 1.0, &vertices,
                                      NULL, NULL, &indices );
    // Load the vertex position 
    glVertexAttribPointer(ver, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(ver);
    
    // Load the MVP matrix
    glUniformMatrix4fv ( mvpLoc, 1, GL_FALSE, ( GLfloat * ) &mvpMatrix.m[0][0] );
    
    // Set the vertex color to green
    glVertexAttrib4f ( 1, 0.0f, 1.0f, 0.0f, 1.0f );


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements ( GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices );
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
