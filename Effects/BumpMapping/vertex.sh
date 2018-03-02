#version 300 es

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform vec4 lightPosition;
uniform vec3 eyePosition;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;

out vec3 vertex_to_fragment_normal;
out vec3 vertex_in_eye_coord;
out vec3 LightPos_in_Normal_coord;
out vec3 EyePos_in_Normal_coord;
out vec2 v_texCoord;

void main() 
{ 
	vertex_in_eye_coord = vec3(mvMatrix * vec4(a_position, 1.0));
	vec3 lightPosition_in_eye_coord = vec3(mvMatrix * lightPosition);

	// Light & Eye position needs to be transformed into Tangent Space. 
	// From the Normal vector have to derive the tangent vector first.
	// In the case of a plane parallel to x-y plane normalized 
      	// z axis co-ordinates is equal to surface normals

	vec3 n = normalMatrix*vec3(0.0, 0.0, 1.0);
	vec3 c1 = cross(n, vec3(0.0, 0.0, 1.0));
        vec3 c2 = cross(n, vec3(0.0, 1.0, 0.0));
        vec3 t;
        if(length(c1)>length(c2))
        {
                t = c1;
        }
        else
        {
                t = c2;
        }
        t = normalize(t);

	vec3 b = cross(n,t);  // bitangent vector

	LightPos_in_Normal_coord.x = dot(lightPosition_in_eye_coord, t); 
	LightPos_in_Normal_coord.y = dot(lightPosition_in_eye_coord, b); 
	LightPos_in_Normal_coord.z = dot(lightPosition_in_eye_coord, n);
        LightPos_in_Normal_coord   = normalize(LightPos_in_Normal_coord);  

   	EyePos_in_Normal_coord.x = dot(eyePosition, t);
   	EyePos_in_Normal_coord.y = dot(eyePosition, b);
   	EyePos_in_Normal_coord.z = dot(eyePosition, n);
	EyePos_in_Normal_coord   = normalize(EyePos_in_Normal_coord);

	// In the case of a plane parallel to x-y plane normalized 
      	// z axis co-ordinates is equal to surface normals
      	vertex_to_fragment_normal = normalMatrix*vec3(0.0, 0.0, 1.0);
      	// Set the position of the current vertex
      	v_texCoord = a_texCoord;
      	gl_Position = mvpMatrix * vec4(a_position, 1.0);

}    
