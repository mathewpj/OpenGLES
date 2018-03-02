
#ifndef __PLANE_H
#define __PLANE_H

#include <cstring>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

class plane_object{

public:
	plane_object();
	GLfloat* getVertices(){
		return(vertices);
	}
	GLfloat* getTexCoords(){
		return(&vertices[3]);
	}
	GLushort* getIndices(){
		return(indices);
	}
private:
	GLfloat vertices[20];
	GLushort indices[6];
};
#endif
