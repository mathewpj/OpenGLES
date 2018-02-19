#version 130

uniform vec3 LightPos;
uniform float shininess;

// Material properties 
uniform vec3 mambient;
uniform vec3 mdiffuse;
uniform vec3 mspecular;

//Light properties
uniform vec3 lambient;
uniform vec3 ldiffuse;
uniform vec3 lspecular;


in vec3 vertex_to_fragment_normal; // Passed in from VS
in vec3 vertex_in_eye_coord;	   // Passed in from VS	
in vec3 tangentSurface2light;	   // Passed in from VS
in vec3 tangentSurface2view;	   // Passed in from VS	

in vec2 texcoord_vtx_fra;
uniform sampler2D Texture1;

uniform sampler2D Normalmap;


void main() {

vec3 vertex_to_light;
vec3 norm;

float diffuse_mult;
vec3 diffuse_comp;

vec3 reflected_ray;
float spec_mult;
vec3 normalize_eye_ray;
vec3 eye_ray;
vec3 specular_comp;


vec3 texcolor;
texcolor = vec3(texture(Texture1, texcoord_vtx_fra));


// Ambient component 
//vec3 ambient = texcolor*lambient;
vec3 ambient = mambient*lambient;

// Diffuse component 
// Since the unit normal vector can be -1<|x|<1
norm = normalize(texture(Normalmap, texcoord_vtx_fra).xyz*2.0 - 1.0);
vertex_to_light = normalize(tangentSurface2light);
diffuse_mult = max(dot(norm, vertex_to_light), 0.0);
diffuse_comp = diffuse_mult*texcolor*ldiffuse;

// Specular component
reflected_ray = normalize(reflect(-vertex_to_light, norm));
// The Eye Position is considered at the origin
normalize_eye_ray = normalize(tangentSurface2view);
spec_mult = max(dot(reflected_ray, normalize_eye_ray), 0.0);
spec_mult = pow(spec_mult, shininess);

specular_comp = spec_mult*lspecular*mspecular;


gl_FragColor = vec4(texcolor, 1.0) +vec4(ambient + diffuse_comp + specular_comp, 1.0); 




}
