/***************************************************************** 
 *  
 *  OpenGLES 3.0 Program that implements ambient + diffuse + 
 *  specular lighting in the fragment shader. To keep things simple
 *  I'm using a plane. This makes calculaton of normals simple.
 *  This program uses glm library for matrix manipulations. 
 *
 *  -Ensure to enable GL_DEPTH_TEST * set glDepthFunc(GL_LEQUAL)    
 *
 *  sudo apt-get install libglfw3-dev libgles2-mesa-dev
 *  
 *  g++ Lighting.cpp shader.cpp -g -I <path to glm> -lGLESv2 
 *  -lglfw -lm -o lighting
 *
 *  Author : Mathew P Joseph
 *  email  : mathew.p.joseph@gmail.com
 *
 *****************************************************************/

#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include "shader.h"

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

static const GLuint WIDTH  = 500;
static const GLuint HEIGHT = 500;

    GLuint shader_program;
    GLFWwindow* window;
    
    // Uniform locations
    GLint  mvpLoc;
    GLint  mvLoc;
    GLint  normalLoc;
    GLint  l_ambientLoc;
    GLint  m_ambientLoc;
    GLint  l_diffuseLoc;
    GLint  m_diffuseLoc;
    GLint  l_specularLoc;
    GLint  m_specularLoc;
    GLint  light_Loc;
    GLint  surfnormal_Loc;
    GLint  shineLoc;
    
   // Shader "IN" locations	
    GLint ver, nor, tex;

static const GLfloat vVertices[] = {
                            -3.0f, -3.0f, -6.0f,  // Position 0
                             3.0f, -3.0f, -6.0f,  // Position 1
                             3.0f,  3.0f, -6.0f,  // Position 2
                            -3.0f,  3.0f, -6.0f,  // Position 3
                         };
GLushort indices[] = { 1, 2, 0, 0, 2, 3 };

Shader shader;

void SetLightParameters(GLuint shader_id)
{

    ver = glGetAttribLocation(shader.id(), "a_position");
    nor = glGetAttribLocation(shader.id(), "a_normal");
    light_Loc = glGetUniformLocation (shader.id(), "lightPosition" );
    surfnormal_Loc = glGetUniformLocation (shader.id(), "surfaceNormal" );
    l_ambientLoc = glGetUniformLocation (shader.id(), "lambient" );
    m_ambientLoc = glGetUniformLocation (shader.id(), "mambient" );
    l_diffuseLoc = glGetUniformLocation (shader.id(), "ldiffuse" );
    m_diffuseLoc = glGetUniformLocation (shader.id(), "mdiffuse" );
    l_specularLoc = glGetUniformLocation (shader.id(), "lspecular" );
    m_specularLoc = glGetUniformLocation (shader.id(), "mspecular" );
    mvLoc = glGetUniformLocation (shader.id(), "mvMatrix" );
    mvpLoc = glGetUniformLocation (shader.id(), "mvpMatrix" );
    normalLoc = glGetUniformLocation (shader.id(), "normalMatrix" );
    shineLoc = glGetUniformLocation (shader.id(), "shininess" );

    // Load the Light Position
    // Light Position in Model space
    glm::vec3 lPosition = glm::vec3(1.0, 1.0, 1.0);
    glUniform3fv (light_Loc , 1, ( GLfloat * ) &lPosition);
    
    // Load the Surface Normal Vector in Model Space
    glm::vec3 surfNormal = glm::vec3(0.0, 0.0, 1.0);
    glUniform3fv (surfnormal_Loc , 1, ( GLfloat * ) &surfNormal);
     
    // Load the light ambient properties
    glm::vec3 lAmbient = glm::vec3(0.2, 0.0, 0.0);
    glUniform3fv (l_ambientLoc , 1, ( GLfloat * ) &lAmbient);
    	
    // Load the material ambient properties
    glm::vec3 mAmbient = glm::vec3(0.1, 0.0, 0.0);
    glUniform3fv (m_ambientLoc , 1, ( GLfloat * ) &mAmbient);
    
    // Load the light diffuse properties
    glm::vec3 lDiffuse = glm::vec3(1.0, 0.0, 0.0);
    glUniform3fv (l_diffuseLoc , 1, ( GLfloat * ) &lDiffuse);
    	
    // Load the material diffuse properties
    glm::vec3 mDiffuse = glm::vec3(0.1, 0.6, 0.6);
    glUniform3fv (m_diffuseLoc , 1, ( GLfloat * ) &mDiffuse);
    
    // Load the light specular properties
    glm::vec3 lSpecular = glm::vec3(1.0, 0.0, 0.0);
    glUniform3fv (l_specularLoc , 1, ( GLfloat * ) &lSpecular);
    	
    // Load the material specular properties
    glm::vec3 mSpecular = glm::vec3(1.0, 1.0, 0.0);
    glUniform3fv (m_specularLoc , 1, ( GLfloat * ) &mSpecular);
    	 
    // Load the material shininess properties
    GLfloat Shine = 32.0;
    glUniform1f (shineLoc , Shine);
   
    return; 
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

    glEnable ( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glCullFace( GL_BACK );	  
    printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
    printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );

    // 1) Generate Model Matrix         
    glm::mat4 ModelMatrix = glm::mat4(1.0f);

    // 2) Generate View Matrix  
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 ViewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);

    // 3) Generate Projection Matrix 
    // Uncomment below line for Orthographic projection    
    //glm::mat4 Projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f); 
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(60.0f),
                           (float) WIDTH / (float)HEIGHT, 1.0f, 20.0f);

    // 5) Generate the Model-View Matrix
    glm::mat4 MV = ViewMatrix*ModelMatrix;

    // 6) Generate the Normal Matrix
    glm::mat3 NM = glm::transpose(glm::inverse(glm::mat3(MV)));
    //std::cout << glm::to_string(NM) << std::endl;
    
    // 7) Generate the Model-View-Projection Matrix
    glm::mat4 MVP = Projection*ViewMatrix*ModelMatrix;

    // 8) Load the Shaders	
    shader.init("vertex.sh", "fragment.sh");
	
    // 9) Set the ViewPort  
    glViewport(0, 0, WIDTH, HEIGHT);

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // Enable the shaders. This step needs to be done before 
    // loading the uniform variables & passing "in" variables 
    // to shader
    glUseProgram(shader.id());
 
    // 10) Set the light and material properties
    SetLightParameters(shader.id());

    // 11) Pass the vertex VBO
    glVertexAttribPointer(ver, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(ver);
    
    // Load the MV matrix
    glUniformMatrix4fv ( mvLoc, 1, GL_FALSE, ( GLfloat * ) &MV[0][0] );
    // Load the MVP matrix
    glUniformMatrix4fv ( mvpLoc, 1, GL_FALSE, ( GLfloat * ) &MVP[0][0] );
    // Load the Normal matrix
    glUniformMatrix3fv ( normalLoc, 1, GL_FALSE, ( GLfloat * ) &NM[0][0] );

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
