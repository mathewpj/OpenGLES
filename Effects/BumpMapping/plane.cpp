#include "plane.h"

plane_object::plane_object()
{
	// Admittedly not the best way to initialize vertices ..

	GLfloat vVertices[20] = {
		-4.0f, -4.0f, 0.0f,  // Position 0
		 0.0f,  0.0f,        // TexCoord 0 
		 4.0f, -4.0f, 0.0f,  // Position 1
		 0.0f,  1.0f,        // TexCoord 1
		 4.0f,  4.0f, 0.0f,  // Position 2
		 1.0f,  1.0f,        // TexCoord 2
		-4.0f,  4.0f, 0.0f,  // Position 3
		 1.0f,  0.0f         // TexCoord 3
	};

	GLushort vIndices[6] = { 1, 2, 0, 0, 2, 3 };

	std::memcpy(vertices, vVertices, 20*sizeof(GLfloat));
	std::memcpy(indices, vIndices, 6*sizeof(GLushort));
}
