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
 *  g++ BumpMapping.cpp shader.cpp -g -I <path to glm> -lGLESv2 
 *  -lglfw -lm -o bumpmap
 *
 *  Author : Mathew P Joseph
 *  email  : mathew.p.joseph@gmail.com
 *
 *****************************************************************/
// g++ Lighting.cpp shader.cpp readBMP.cpp plane.cpp light.cpp -I /home/mathew/SelfStudy/glm -DDEBUG_ENABLE -g -lGLESv2 -lglfw -lm -o texture

#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include "shader.h"
#include "plane.h"
#include "light.h"
#include "readBMP.h"


#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

static const GLuint WIDTH  = 600;
static const GLuint HEIGHT = 600;


    GLuint shader_program;
    GLFWwindow* window;
    
    // Uniform locations
    GLint  mvpLoc;
    GLint  mvLoc;
    GLint  normalLoc;
    GLint  surfnormal_Loc;
    
   // Shader "IN" locations	
    GLint ver, tex;

Shader shader;
GLuint textureId_main;
GLuint textureId_Normal;

void CreateSimpleTexture2D(char* texture_file, GLuint *textureId)
{
   //GLuint textureId;
   struct Image image_main;
   ImageLoad(texture_file, &image_main);
   char *pixels = image_main.data;

   // Use tightly packed data
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

   // Generate a texture object
   glGenTextures ( 1, textureId );

   // Bind the texture object
   glBindTexture ( GL_TEXTURE_2D, *textureId );

   // Load the texture
   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, image_main.sizeX, image_main.sizeY,
                  0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

   // Set the filtering mode
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
   // Set the texture wrap around mode
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   // Generate a texture object
   glGenTextures ( 1, textureId );
   delete pixels;

   return;

}

int main(int argc, char* argv[]) {

    plane_object Plane;
    light_source Light;
    light_material Material;
    GLuint brick_tex;
    GLuint brick_normal_tex;
    GLuint eye_Loc;
    GLfloat angle = 20.0;
	

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
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 ViewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);

    // 3) Generate Projection Matrix 
    // Uncomment below line for Orthographic projection    
    //glm::mat4 Projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f); 
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(60.0f),
                           (float) WIDTH / (float)HEIGHT, 0.5f, 20.0f);

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
 
    // 10) Set the light and material properties & pass eye 
    // position to shader
    Light.getUniformLocations(shader.id());
    Light.setLightParams();
    Material.getUniformLocations(shader.id());
    Material.setMaterialParams();

    eye_Loc = glGetUniformLocation (shader.id(), "ePosition" );
    glUniform3fv (eye_Loc , 1, ( GLfloat * ) &cameraPosition);


    // 11) Pass multiple texture is tricky 
    //    First activate the texture to use 
    char BrickTexture[] = "brickwork.bmp";
    glActiveTexture(GL_TEXTURE0);
    CreateSimpleTexture2D (BrickTexture, &textureId_main);
    brick_tex = glGetUniformLocation(shader.id(), "Texture");
    //Set our "earth_tex" sampler to use Texture Unit 0
    glUniform1i(brick_tex, 0);
    
    //  Activate second texture unit for Normal map	
    glActiveTexture(GL_TEXTURE1);
    char BrickNormalTexture[] = "brickwork_normal_map.bmp";
    CreateSimpleTexture2D (BrickNormalTexture, &textureId_Normal);
    brick_normal_tex = glGetUniformLocation(shader.id(), "Normal_Texture");
    // Set our "earth_stencil_tex" sampler to use Texture Unit 1
    glUniform1i(brick_normal_tex, 1);
  	

    // 12) Pass the vertex VBO
    ver = glGetAttribLocation(shader.id(), "a_position");
    tex = glGetAttribLocation(shader.id(), "a_texCoord");
    glVertexAttribPointer ( ver, 3, GL_FLOAT,
                           GL_FALSE, 5 * sizeof ( GLfloat ), Plane.getVertices());
    glEnableVertexAttribArray(ver);
    glVertexAttribPointer ( tex, 2, GL_FLOAT,
                           GL_FALSE, 5 * sizeof ( GLfloat ), Plane.getTexCoords());
    glEnableVertexAttribArray(tex);
    // Load the MVP matrix
    glUniformMatrix4fv ( mvpLoc, 1, GL_FALSE, ( GLfloat * ) &MVP[0][0] );
    // Load the MVP matrix
    glUniformMatrix4fv ( mvpLoc, 1, GL_FALSE, ( GLfloat * ) &MVP[0][0] );
    // Load the Normal matrix
    glUniformMatrix3fv ( normalLoc, 1, GL_FALSE, ( GLfloat * ) &NM[0][0] );


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, Plane.getIndices());
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
