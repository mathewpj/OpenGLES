#version 300 es 

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform vec3 lightPosition;
uniform vec3 surfaceNormal;

layout(location = 0) in vec3 a_position;

out vec3 vertex_to_fragment_normal;
out vec3 vertex_in_eye_coord;
out vec3 LightPos_in_eye_coord;

void main()
{
      vertex_in_eye_coord = vec3(mvMatrix * vec4(a_position, 1.0));
      LightPos_in_eye_coord = vec3(mvMatrix * vec4(lightPosition, 1.0));
      vertex_to_fragment_normal = normalMatrix*surfaceNormal;
      // Set the position of the current vertex
      gl_Position = mvpMatrix * vec4(a_position, 1.0);
}
