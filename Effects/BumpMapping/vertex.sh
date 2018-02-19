#version 130
in  vec2 TexCoord0;
out vec2 texcoord_vtx_fra;

uniform vec3 LightPos;

in vec3 app_to_vertex_normal;
out vec3 vertex_to_fragment_normal;
out vec3 vertex_in_eye_coord;
out vec3 tangentSurface2light;
out vec3 tangentSurface2view;

void main() {			

	// Set the position of the current vertex 
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	texcoord_vtx_fra = TexCoord0.xy;

	vertex_in_eye_coord = vec3(gl_ModelViewMatrix * gl_Vertex);

	vertex_to_fragment_normal = gl_NormalMatrix*gl_Normal;
	
        vec3 TempLightPos = LightPos - vec3(gl_Vertex);
	vec3 tangent; 
	vec3 n = normalize(vertex_to_fragment_normal);
	//vec3 t = normalize(gl_NormalMatrix*vec3(1.0, 0.0, 0.0));

        vec3 c1 = cross(gl_Normal, vec3(0.0, 0.0, 1.0)); 
	vec3 c2 = cross(gl_Normal, vec3(0.0, 1.0, 0.0)); 
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

	vec3 b = cross(n,t);
	// Tangent space - hence tangent in tangentSurface2light
	tangentSurface2light.x = dot(TempLightPos, t);	
	tangentSurface2light.y = dot(TempLightPos, b);	
	tangentSurface2light.z = dot(TempLightPos, n);	
	tangentSurface2light = normalize(tangentSurface2light);
	
	tangentSurface2view.x = dot(-vertex_in_eye_coord, t);
	tangentSurface2view.y = dot(-vertex_in_eye_coord, b);
	tangentSurface2view.z = dot(-vertex_in_eye_coord, b);
	tangentSurface2view = normalize(tangentSurface2view);

}
