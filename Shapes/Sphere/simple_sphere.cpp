/***************************************************************** 
 *  
 *  OpenGLES 3.0 Program to draw a sphere with OpenGL Matix (glm) 
 *  library 
 *
 *  -Ensure to enable GL_DEPTH_TEST * set glDepthFunc(GL_LEQUAL)    
 *  -Ensure to clear the Depth buffer for every key press
 *  -use glm version 0.9.8.5 
 *
 *  sudo apt-get install libglfw3-dev libgles2-mesa-dev
 *  g++ simple_sphere.cpp sphere_gen.cpp load_shader.cpp 
 *      -I ./glm OR <path to glm> -lGLESv2 -lglfw -lm -o Sphere  
 *
 *  Author : Mathew P Joseph
 *  email  : mathew.p.joseph@gmail.com
 *
 *****************************************************************/

#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
using namespace std;

extern int esGenSphere ( int numSlices, float radius, GLfloat **vertices, GLfloat **normals,
                             GLfloat **texCoords, GLuint **indices );
extern GLint common_get_shader_program(const char *vertex_shader_source, 
			     const char *fragment_shader_source);


   
static const GLchar* vertex_shader_source =
      "#version 300 es                             \n"
      "uniform mat4 u_mvpMatrix;                   \n"
      "layout(location = 0) in vec4 a_position;    \n"
      "void main()                                 \n"
      "{                                           \n"
      "   gl_Position = u_mvpMatrix * a_position;  \n"
      "}        				   \n"; 
static const GLchar* fragment_shader_source =
      "#version 300 es                             \n"
      "precision mediump float;                    \n"
      "layout(location = 0) out vec4 outColor;     \n"
      "void main()                                 \n"
      "{                                           \n"
      "  outColor = vec4(1.0, 0.0, 0.0, 1.0);      \n"
      "}                                           \n";

int main(int argc, char* argv[]) {
    const GLuint WIDTH  = 500;
    const GLuint HEIGHT = 500;
    GLuint shader_program;
    GLint numIndices;
    GLFWwindow* window;
    
    // Vertex data
    GLfloat  *vertices;
    GLuint *indices;
    
    // Uniform locations
    GLint  mvpLoc;
    // Shader "IN" locations	
    GLint ver;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, NULL, NULL);
    if(window==NULL){	
	cout << "glfwCreateWindow Error"<<endl;
	return -1;
    }		
    glfwMakeContextCurrent(window);

    glEnable ( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glCullFace( GL_BACK );	  
    cout << "GL_VERSION  : " << endl << glGetString(GL_VERSION) << endl;
    cout << "GL_RENDERER : " << glGetString(GL_RENDERER) << endl;

    // 1) Generate Model Matrix 	
    glm::mat4 ModelMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -30.0f));

    // 2) Generate View Matrix 	
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -2.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 ViewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);

    // 3) Generate Projection Matrix 
    // Uncomment below two lines for Orthographic projection	
    // glm::mat4 Projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f); 
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 
                           (float) WIDTH / (float)HEIGHT, 0.1f, 100.0f);
  
    // 4) Generate the Model-View-Projection Matrix
    glm::mat4 MVP = Projection*ViewMatrix*ModelMatrix;

    // 5) Load the Shaders    
    shader_program = common_get_shader_program(vertex_shader_source, fragment_shader_source);
 
    // 6) Set the ViewPort  
    glViewport(0, 0, WIDTH, HEIGHT);

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Enable the shaders. This step needs to be done before 
    // loading the uniform variables & passing "in" variables 
    // to shader
    glUseProgram(shader_program);

    numIndices = esGenSphere ( 100, 1.0f, &vertices, NULL, NULL, &indices );
    // Get the uniform & "in" variable locations
    ver = glGetAttribLocation(shader_program, "a_position");
    mvpLoc = glGetUniformLocation ( shader_program, "u_mvpMatrix" );
  
    glVertexAttribPointer(ver, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(ver);
 
    // Load the MVP matrix
    glUniformMatrix4fv ( mvpLoc, 1, GL_FALSE, ( GLfloat * ) &MVP[0][0] );

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        // 7) Draw the triangles
        glDrawElements ( GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices );
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}


