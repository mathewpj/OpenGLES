#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "imageloader.h"

#include "shader.h"

//g++ Main.cpp Shader.cpp imageloader.cpp -g -lGL -lGLU -lglut -lGLEW

Shader shader;

unsigned int myImg1, myImg2;
float light_position[3] = {1.0, 1.0, 1.0};


//Makes the image into a texture, and returns the id of the texture
GLuint _textureId1, _textureId2; //The id of the texture
GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit

	printf("\nIn loadTexture width = %d height = %d\n",image->width, image->height), 
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		
		//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
				 0,                            //0 for now
				 GL_RGB,                       //Format OpenGL uses for image
				 image->width, image->height,  //Width and height
				 0,                            //The border of the image
				 GL_RGB, //GL_RGB, because pixels are stored in RGB format
				 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
				                   //as unsigned numbers
				 image->pixels);               //The actual pixel data
	return textureId; //Returns the id of the texture
}




void init(void){
	Image *image1, *image2;  
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glMatrixMode(GL_MODELVIEW);
	shader.init("vertex.sh", "fragment.sh");
	image1 = loadBMP("brickwork.bmp");
	myImg1 = loadTexture(image1);
	image2 = loadBMP("brickwork_normal_map.bmp");
	myImg2 = loadTexture(image1);
	delete image2; 
	delete image1;
}

void display(void){
	float x = 0.0, y = 0.0, z = 0.0;
	GLint texLoc, texCoordLoc;
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	shader.bind();
	{
		// If you are using normal maps, you shoudnt be sending the normal 
		// at the vertices. The Normal map does that for you.
		// "in" variables to vertex shader
			glVertexAttrib3f(glGetAttribLocation(shader.id(), "app_to_vertex_normal"), 
			0.0, 0.0, 1.0);			// need to specify only once since the 
							// normal at all 4 vertexs' are the same
	}
	{
		// uniform variables to fragment shader 
		x += light_position[0];
		y += light_position[1];
		z += light_position[2];

		glUniform3f(glGetUniformLocation(shader.id(), "LightPos"), x, y, z);
		glUniform1f(glGetUniformLocation(shader.id(), "shininess"), 512.0);
		// Setting the Material properties
		glUniform3f(glGetUniformLocation(shader.id(), "mambient"), 0.2, 0.2, 0.2); 
		glUniform3f(glGetUniformLocation(shader.id(), "mdiffuse"), 0.6, 0.6, 0.6); 
		glUniform3f(glGetUniformLocation(shader.id(), "mspecular"), 1.0, 1.0, 1.0); 
		// Setting the Light properties
		//glUniform3f(glGetUniformLocation(shader.id(), "lambient"), 0.2, 0.2, 0.2); 
		glUniform3f(glGetUniformLocation(shader.id(), "lambient"), 0.0, 0.0, 0.0); 
		glUniform3f(glGetUniformLocation(shader.id(), "ldiffuse"), 0.6, 0.6, 0.6); 
		glUniform3f(glGetUniformLocation(shader.id(), "lspecular"), 1.0, 1.0, 1.0); 

		
		glActiveTexture(GL_TEXTURE0); // GL_TEXTURE0 -> the texture unit
		glBindTexture(GL_TEXTURE_2D, myImg1);
		glActiveTexture(GL_TEXTURE1); // GL_TEXTURE0 -> the texture unit
		glBindTexture(GL_TEXTURE_2D, myImg2);


	}
	
	

	texLoc = glGetUniformLocation(shader.id(), "Texture1");		// TextureUnit
	texCoordLoc = glGetAttribLocation(shader.id(), "TexCoord0");
	printf("\n texLoc =  %d , texCoordLoc = %d\n", texLoc, texCoordLoc);
	// Mathew - Not sure why texLoc shows a junk value !
	glUniform1i(texLoc, 0);

	texLoc = glGetUniformLocation(shader.id(), "Normalmap");		// TextureUnit
	printf("\n Normalmap texLoc =  %d\n", texLoc);
	glUniform1i(texLoc, 1);

	glBegin(GL_QUADS);
//	glTexCoord2f(0, 0);		// Mathew - need not pass the texture co-ord to vertex mapping.
					// this is passed as the "in" attribute TexCoord0
	glVertexAttrib2f(texCoordLoc, 0.0, 0.0);
	glVertex3f(-5.0, -5.0, -1.0);

//	glTexCoord2f(1, 0);
	glVertexAttrib2f(texCoordLoc, 1.0, 0.0);
	glVertex3f(5.0, -5.0, -1.0);

//	glTexCoord2f(1, 1);
	glVertexAttrib2f(texCoordLoc, 1.0, 1.0);
	glVertex3f(5.0, 5.0, -1.0);

//	glTexCoord2f(0, 1);
	glVertexAttrib2f(texCoordLoc, 0.0, 1.0);
	glVertex3f(-5.0, 5.0, -1.0);
	
	glEnd();
	shader.unbind();

	glutSwapBuffers();
}


// Whenever you initially open a window or later move or resize that window, 
// the window system will send an event to notify you. If you are using GLUT, 
// the notification is automated; whatever routine has been registered to 
// glutReshapeFunc() will be called. You must register a callback function 
// that will, 
// (1) Reestablish the rectangular region that will be the new rendering canvas 
// (2) Define the coordinate system to which objects will be drawn
// 1 done by -> glViewport(0, 0, (GLsizei)w, (GLsizei)h);
// 2 done by -> gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);

void reshape(int w, int h){
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
}


// ----------------------------------------------------------
// specialKeys() Callback Function
// ----------------------------------------------------------
void specialKeys( int key, int x, int y ) {
  
  if (key == GLUT_KEY_RIGHT){
   	light_position[0] = 0.2;
	light_position[1] = 0.2;
	light_position[2] = 0.2;}
  else if (key == GLUT_KEY_LEFT){
	light_position[0] = -0.2;
	light_position[1] = -0.2;
	light_position[2] = -0.2;}
  
  //  Request display update
  glutPostRedisplay();
 
}



int main(int argc, char** argv){
	GLenum glErr;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Rendering a texture with GLSL");

	glewInit();
	glErr = glGetError();
	//printf("\nglewInit : %s\n", gluErrorString(glErr));
#if 1	
	if (GL_TRUE != glewGetExtension((const char*) "GL_ARB_fragment_shader"))
	  printf("GL_ARB_fragment_shader extension is not available!\n");
	
	if (GL_TRUE != glewGetExtension((const char*) "GL_ARB_vertex_shader"))
	  printf("GL_ARB_vertex_shader extension is not available!\n");

	if (GL_TRUE != glewGetExtension((const char*) "GL_ARB_shader_objects"))
	  printf("GL_ARB_shader_objects extension is not available!\n");
#endif


	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(specialKeys);
	
	init();

	glutMainLoop();

	return 0;
}
