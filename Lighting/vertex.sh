#version 300 es 

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 lightPosition;
out vec3 vertex_to_fragment_normal;
out vec3 vertex_in_eye_coord;
out vec3 LightPos_in_eye_coord;
void main()
{
      vertex_in_eye_coord = vec3(mvMatrix * a_position);
      LightPos_in_eye_coord = vec3(mvMatrix * lightPosition);
      vertex_to_fragment_normal = normalMatrix*vec3(0.0, 0.0, 1.0);
      // Set the position of the current vertex
      gl_Position = mvpMatrix * a_position;
}
