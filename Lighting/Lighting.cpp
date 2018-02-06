/***************************************************************** 
 *  
 *  OpenGLES 3.0 Program that implements ambient + diffuse + 
 *  specular lighting in the fragment shader. To keep things simple
 *  I'm using a plane. This makes calculaton of normals simple.
 *  This program uses glm library for matrix manipulations. 
 *
 *  -Ensure to enable GL_DEPTH_TEST * set glDepthFunc(GL_LEQUAL)    
 *  -Ensure to clear the Depth buffer for every key press
 *
 *  sudo apt-get install libglfw3-dev libgles2-mesa-dev
 *  g++ Lighting.cpp load_shader.cpp -g -I ./glm -lGLESv2 
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


#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

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
    GLint  shineLoc;
    // Shader "IN" locations	
    GLint ver, nor, tex;
   
    GLfloat  aspect;
    GLfloat  angle;

static const GLfloat vVertices[] = {
                            -3.0f, -3.0f, -6.0f,  // Position 0
                             3.0f, -3.0f, -6.0f,  // Position 1
                             3.0f,  3.0f, -6.0f,  // Position 2
                            -3.0f,  3.0f, -6.0f,  // Position 3
                         };
GLushort indices[] = { 1, 2, 0, 0, 2, 3 };


// Note that the vector has to be multiplied to the matrix from the right.
static const GLchar* vertex_shader_source =
      "#version 300 es                            		\n"
      "uniform mat4 mvpMatrix;					\n"		
      "uniform mat4 mvMatrix;					\n"		
      "uniform mat3 normalMatrix;				\n"
      "layout(location = 0) in vec4 a_position;			\n" 
      "out vec3 vertex_to_fragment_normal;			\n"
      "out vec3 vertex_in_eye_coord;				\n"
      "out vec3 LightPos_in_eye_coord;				\n"
      "void main()                                		\n"
      "{							\n"
      "  mat3 NM = mat3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);	 \n"
      "  vec4 LightPos_intermediate = vec4(0.0, 0.0, 0.0, 1.0); \n"
      "	 // Set the position of the current vertex		\n"
      "  gl_Position = mvpMatrix * a_position;			\n"
      "  vertex_in_eye_coord = vec3(mvMatrix * a_position);	\n"
      "  LightPos_in_eye_coord = vec3(mvMatrix * LightPos_intermediate); \n"
      "  vertex_to_fragment_normal = normalMatrix*vec3(0.0, 0.0, 1.0); \n"
      "  //vertex_to_fragment_normal = NM*vec3(0.0, 0.0, 1.0);    \n"
      "}                                          		\n";
static const GLchar* fragment_shader_source =
      "#version 300 es				  		\n"
      "precision mediump float;			  		\n"
      "in vec3 vertex_to_fragment_normal;  			\n"
      "in vec3 vertex_in_eye_coord;				\n"
      "in vec3 LightPos_in_eye_coord;				\n" 
      "layout(location = 0) out vec4 outColor;	  		\n"
      "uniform float shininess;					\n"
      "uniform vec3 mambient;					\n"
      "uniform vec3 mdiffuse;					\n"
      "uniform vec3 mspecular;					\n"
      "uniform vec3 lambient;					\n"
      "uniform vec3 ldiffuse;					\n"
      "uniform vec3 lspecular;					\n"
      "void main() 						\n"
      "{					  		\n"
      "vec3 vertex_to_light;					\n"
      "vec3 norm;						\n"
      "float diffuse_mult;					\n"
      "vec3 diffuse_comp;					\n"
      "vec3 reflected_ray;					\n"
      "float spec_mult;						\n"
      "vec3 normalize_eye_ray;					\n"
      "vec3 eye_ray;						\n"
      "vec3 specular_comp;					\n"
      "// Ambient component					\n"
      "vec3 ambient = lambient*mambient;			\n"
      "// Diffuse component					\n"
      "norm = normalize(vertex_to_fragment_normal);		\n"
      "vertex_to_light = normalize(LightPos_in_eye_coord - vertex_in_eye_coord); \n"
      "diffuse_mult = max(dot(norm, vertex_to_light), 0.0);     \n"
      "diffuse_comp = diffuse_mult*ldiffuse*mdiffuse;		\n"
      "// Specular component					\n"
      "reflected_ray = normalize(reflect(-vertex_to_light, norm));     \n"
      "// The Eye Position is considered at the origin	        \n"
      "eye_ray = vec3(0.0, 0.0, 0.0) - vertex_in_eye_coord;     \n"
      "normalize_eye_ray = normalize(eye_ray);			\n"
      "spec_mult = max(dot(reflected_ray, normalize_eye_ray), 0.0);    \n"
      "spec_mult = pow(spec_mult, 16.0);			\n"	
      "specular_comp = spec_mult*lspecular*mspecular;		\n"
      "outColor = vec4( ambient + diffuse_comp + specular_comp, 1.0 ); \n"
      "}					  		\n";


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
    //glShadeModel(GL_SMOOTH );
    printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
    printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );

    // 1) Generate Model Matrix         
    //glm::mat4 ModelMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
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
    shader_program = common_get_shader_program(vertex_shader_source, fragment_shader_source);

    // 9) Set the ViewPort  
    glViewport(0, 0, WIDTH, HEIGHT);

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // Enable the shaders. This step needs to be done before 
    // loading the uniform variables & passing "in" variables 
    // to shader
    glUseProgram(shader_program);

 
    // Get the uniform & "in" variable locations
    ver = glGetAttribLocation(shader_program, "a_position");
    nor = glGetAttribLocation(shader_program, "a_normal");
    light_Loc = glGetUniformLocation ( shader_program, "LightPosition" );
    l_ambientLoc = glGetUniformLocation ( shader_program, "lambient" );
    m_ambientLoc = glGetUniformLocation ( shader_program, "mambient" );
    l_diffuseLoc = glGetUniformLocation ( shader_program, "ldiffuse" );
    m_diffuseLoc = glGetUniformLocation ( shader_program, "mdiffuse" );
    l_specularLoc = glGetUniformLocation ( shader_program, "lspecular" );
    m_specularLoc = glGetUniformLocation ( shader_program, "mspecular" );
    mvLoc = glGetUniformLocation ( shader_program, "mvMatrix" );
    mvpLoc = glGetUniformLocation ( shader_program, "mvpMatrix" );
    normalLoc = glGetUniformLocation ( shader_program, "normalMatrix" );
    shineLoc = glGetUniformLocation ( shader_program, "shininess" );

 
    //numIndices = esGenSphere ( 100, 2.0f, &vertices, &normals,
    //                          &texcoords, &indices );

    glVertexAttribPointer(ver, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(ver);
    
   // glVertexAttribPointer(nor, 3, GL_FLOAT, GL_FALSE, 0, normals);
   // glEnableVertexAttribArray(nor);
  
    // Load the Light Position
    // Light Position in Model space
    glm::vec3 lPosition = glm::vec4(0.0, 0.0, 0.0, 0.0);
    glUniform3fv (light_Loc , 1, ( GLfloat * ) &lPosition);
    
    // Load the light ambient properties
    glm::vec3 lAmbient = glm::vec3(1.0, 0.0, 0.0);
    glUniform3fv (l_ambientLoc , 1, ( GLfloat * ) &lAmbient);
    	
    // Load the material ambient properties
    glm::vec3 mAmbient = glm::vec3(0.8, 0.2, 0.2);
    glUniform3fv (m_ambientLoc , 1, ( GLfloat * ) &mAmbient);
    
    // Load the light diffuse properties
    glm::vec3 lDiffuse = glm::vec3(1.0, 0.6, 0.6);
    glUniform3fv (l_diffuseLoc , 1, ( GLfloat * ) &lDiffuse);
    	
    // Load the material diffuse properties
    glm::vec3 mDiffuse = glm::vec3(0.6, 0.6, 0.6);
    glUniform3fv (m_diffuseLoc , 1, ( GLfloat * ) &mDiffuse);
    
    // Load the light specular properties
    glm::vec3 lSpecular = glm::vec3(1.0, 1.0, 1.0);
    glUniform3fv (l_specularLoc , 1, ( GLfloat * ) &lSpecular);
    	
    // Load the material specular properties
    glm::vec3 mSpecular = glm::vec3(0.0, 0.0, 0.0);
    glUniform3fv (m_specularLoc , 1, ( GLfloat * ) &mSpecular);
    	 
    // Load the material shininess properties
    GLfloat Shine = 16.0;
    glUniform3fv (shineLoc , 1, ( GLfloat * ) &Shine);
    
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
        //glDrawElements ( GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices );
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
