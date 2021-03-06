#version 300 es

precision mediump float;
in vec3 vertex_in_eye_coord;
in vec3 LightPos_in_Normal_coord;
in vec3 EyePos_in_Normal_coord;
in vec2 v_texCoord;

uniform sampler2D Texture;
uniform sampler2D Normal_Texture;

layout(location = 0) out vec4 outColor;

uniform vec3 lambient;
uniform vec3 ldiffuse;
uniform vec3 lspecular;

uniform vec3 mambient;
uniform vec3 mdiffuse;
uniform vec3 mspecular;

uniform float shininess;

void main()
{
      vec3 vertex_to_light;
      vec3 norm;
      float diffuse_mult;
      vec3 diffuse_comp;
      vec3 reflected_ray;
      float spec_mult;
      vec3 normalize_eye_ray;
      vec3 eye_ray;
      vec3 specular_comp = vec3(0.0, 0.0, 0.0);

      // Ambient component
      vec3 ambient = lambient*mambient;

      // Diffuse component
      // Since the unit normal vector can be -1<|x|<1
      norm = normalize(texture(Normal_Texture, v_texCoord).xyz*2.0 - 1.0);
      vertex_to_light = normalize(LightPos_in_Normal_coord - vertex_in_eye_coord);
      diffuse_mult = max(dot(norm, vertex_to_light), 0.0);
      diffuse_comp = diffuse_mult*ldiffuse*mdiffuse;

      vec3 v2l = normalize(LightPos_in_Normal_coord - vertex_in_eye_coord);
      vec3 v2e = normalize(vec3(0.0, 0.0, 0.0) - vertex_in_eye_coord);


      // Specular component
      reflected_ray = normalize(reflect(-vertex_to_light, norm));
      // The Eye Position is considered at the origin
      eye_ray = LightPos_in_Normal_coord - vertex_in_eye_coord;
      normalize_eye_ray = normalize(eye_ray);
      spec_mult = max(dot(reflected_ray, normalize_eye_ray), 0.0);
      spec_mult = pow(spec_mult, shininess);
      specular_comp = spec_mult*lspecular*mspecular;

      outColor = vec4( ambient + diffuse_comp + specular_comp, 1.0 );
      outColor = outColor + texture(Texture, v_texCoord );
}
   
