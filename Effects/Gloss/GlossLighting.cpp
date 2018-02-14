/***************************************************************** 
 *  
 *  OpenGLES 3.0 Program that implements "Gloss Lighting".
 *  There are two textures, the "main" texture and the "stencil" 
 *  texture. Specular regions arelimitied to the positive regions 
 *  in the stencil texture thereby giving a glossy impression. 
 *
 *  -Ensure to enable GL_DEPTH_TEST * set glDepthFunc(GL_LEQUAL)    
 *
 *  sudo apt-get install libglfw3-dev libgles2-mesa-dev
 *  
 *  g++ GlossLighting.cpp sphere_gen.cpp readBMP.cpp shader.cpp 
 *   -g -I <path to glm> -lGLESv2 -lglfw -lm -o Gloss
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
#include "readBMP.h"

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

int  esGenSphere ( int numSlices, float radius, GLfloat **vertices, GLfloat **normals,
                             GLfloat **texCoords, GLuint **indices );

static const GLuint WIDTH  = 800;
static const GLuint HEIGHT = 800;

    GLuint 	shader_program;
    GLFWwindow* window;

    // Vertex data
    GLfloat  *vertices;
    GLfloat  *texcoords;
    GLuint   *indices;
    GLint    numIndices;
 
    // Uniform locations
    GLint  mvpLoc;
    GLint  mvLoc;
    GLint  normalLoc;
    // Shader "IN" locations	
    GLint ver, tex;
   
    Shader shader;
    GLuint textureId_main;
    GLuint textureId_stencil;

    glm::mat4 ModelMatrix; 
    glm::mat4 Projection;
    glm::mat4 ViewMatrix;
    glm::mat4 MV;
    glm::mat3 NM;
    glm::mat4 MVP;

    struct Image image_stencil;

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

void SetLightParameters(GLuint shader_id)
{
    // Uniform locations
    GLint  l_ambientLoc;
    GLint  m_ambientLoc;
    GLint  l_diffuseLoc;
    GLint  m_diffuseLoc;
    GLint  l_specularLoc;
    GLint  m_specularLoc;
    GLint  light_Loc;
    GLint  shineLoc;

    ver = glGetAttribLocation(shader.id(), "a_position");
    tex = glGetAttribLocation(shader.id(), "a_texCoord");
    light_Loc = glGetUniformLocation (shader.id(), "lightPosition" );
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
    glm::vec3 lPosition = glm::vec4(1.0, 1.0, 1.0, 0.0);
    glUniform4fv (light_Loc , 1, ( GLfloat * ) &lPosition);
    
    // Load the light ambient properties
    glm::vec3 lAmbient = glm::vec3(1.0, 0.0, 0.0);
    glUniform3fv (l_ambientLoc , 1, ( GLfloat * ) &lAmbient);
    	
    // Load the material ambient properties
    glm::vec3 mAmbient = glm::vec3(0.1, 1.0, 1.0);
    glUniform3fv (m_ambientLoc , 1, ( GLfloat * ) &mAmbient);
    
    // Load the light diffuse properties
    glm::vec3 lDiffuse = glm::vec3(1.0, 0.0, 0.0);
    glUniform3fv (l_diffuseLoc , 1, ( GLfloat * ) &lDiffuse);
    	
    // Load the material diffuse properties
    glm::vec3 mDiffuse = glm::vec3(0.1, 0.0, 0.6);
    glUniform3fv (m_diffuseLoc , 1, ( GLfloat * ) &mDiffuse);
    
    // Load the light specular properties
    glm::vec3 lSpecular = glm::vec3(1.0, 1.0, 1.0);
    glUniform3fv (l_specularLoc , 1, ( GLfloat * ) &lSpecular);
    	
    // Load the material specular properties
    glm::vec3 mSpecular = glm::vec3(0.2, 0.2, 0.2);
    glUniform3fv (m_specularLoc , 1, ( GLfloat * ) &mSpecular);
    	 
    // Load the material shininess properties
    GLfloat Shine = 2.0;
    glUniform1f (shineLoc , Shine);
   
    return; 
}

    //!GLFW keyboard callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){

    static GLint entry = 0;
    static GLfloat angle = 0.0f;
    // key_callback somehow gets called twice for every key press. So limiting it to one...	 
    if(entry%3 == 0){
		// Clear the Depth Buffer for each frame!
		glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
		// Rotate the sphere
    		angle += 0.0001f;
    		ModelMatrix = glm::rotate(ModelMatrix, (glm::mediump_float)angle, glm::vec3(0.0f, 1.0f, 0.0f)); 

    		// Compute MV matrix
    		MV = ViewMatrix*ModelMatrix;
    		// Compute NM matrix
    		NM = glm::transpose(glm::inverse(glm::mat3(MV)));
    		// Compute MVP matrix
    		MVP = Projection*ViewMatrix*ModelMatrix;
    		// Load the MV matrix
		glUniformMatrix4fv ( mvLoc, 1, GL_FALSE, ( GLfloat * ) &MV[0][0] );
		// Load the MVP matrix
		glUniformMatrix4fv ( mvpLoc, 1, GL_FALSE, ( GLfloat * ) &MVP[0][0] );
		// Load the Normal matrix
		glUniformMatrix3fv ( normalLoc, 1, GL_FALSE, ( GLfloat * ) &NM[0][0] );
     	}
     entry++;		
} 

int main(int argc, char* argv[]) {
    GLuint earth_tex;
    GLuint earth_stencil_tex;

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

    glEnable ( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glCullFace( GL_BACK );	  
    printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
    printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );



    // 1) Generate Model Matrix        
    ModelMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -4.0f)); 
    //ModelMatrix = glm::mat4(1.0f);

    // 2) Generate View Matrix  
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 2.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    ViewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);

    // 3) Generate Projection Matrix 
    // Uncomment below line for Orthographic projection    
    // Projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f); 
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    Projection = glm::perspective(glm::radians(60.0f), (float) WIDTH / (float)HEIGHT, 2.0f, 20.0f);

    // 5) Generate the Model-View Matrix
    MV = ViewMatrix*ModelMatrix;

    // 6) Generate the Normal Matrix
    NM = glm::transpose(glm::inverse(glm::mat3(MV)));
    //std::cout << glm::to_string(NM) << std::endl;
    
    // 7) Generate the Model-View-Projection Matrix
    MVP = Projection*ViewMatrix*ModelMatrix;

    // 8) Load the Shaders	
    shader.init("vertex.sh", "fragment.sh");
	
    // 9) Set the ViewPort  
    glViewport(0, 0, WIDTH, HEIGHT);

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // 10) Enable the shaders. This step needs to be done before 
    // loading the uniform variables & passing "in" variables to shader
    glUseProgram(shader.id());

    // 11) Passing multiple texture is tricky  
    // First activate the texture to use 
    
    char EarthTexture[] = "earth.bmp";
    glActiveTexture(GL_TEXTURE0); 
    CreateSimpleTexture2D (EarthTexture, &textureId_main);   
    earth_tex = glGetUniformLocation(shader.id(), "earth_texture");
    //Set our "earth_tex" sampler to use Texture Unit 0
    glUniform1i(earth_tex, 0);
    
    glActiveTexture(GL_TEXTURE1); 
    char EarthStencilTexture[] = "earth_stencil.bmp";
    CreateSimpleTexture2D (EarthStencilTexture, &textureId_stencil);   
    earth_stencil_tex = glGetUniformLocation(shader.id(), "earth_stencil_texture");
    // Set our "earth_stencil_tex" sampler to use Texture Unit 1
    glUniform1i(earth_stencil_tex, 1);

 
    // 12) Set the light and material properties
    SetLightParameters(shader.id());
 
    numIndices = esGenSphere ( 50, 2.0f, &vertices, NULL, &texcoords, &indices );

    // 13) Pass the vertex & texure coordinates BO
    glVertexAttribPointer(ver, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(ver);
    glVertexAttribPointer (tex, 2, GL_FLOAT, GL_FALSE, 0, texcoords); 
    glEnableVertexAttribArray (tex);  
    
    // Load the MV matrix
    glUniformMatrix4fv ( mvLoc, 1, GL_FALSE, ( GLfloat * ) &MV[0][0] );
    // Load the MVP matrix
    glUniformMatrix4fv ( mvpLoc, 1, GL_FALSE, ( GLfloat * ) &MVP[0][0] );
    // Load the Normal matrix
    glUniformMatrix3fv ( normalLoc, 1, GL_FALSE, ( GLfloat * ) &NM[0][0] );

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
       // glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
        glDrawElements ( GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices );
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
