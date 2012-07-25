varying vec3 normal;   // vertex normal in eye space.
varying vec3 position; // vertex position in eye space.




void main( void )
{
  
  normal = gl_NormalMatrix * gl_Normal;
  position = ( gl_ModelViewMatrix * gl_Vertex ).xyz;

  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  gl_TexCoord[0] = gl_MultiTexCoord0;

  vec4 eyeCoordPos = gl_ModelViewMatrix * gl_Vertex;
  vec3 pos = vec3(eyeCoordPos) / eyeCoordPos.w;
  vec3 l = vec3(gl_LightSource[0].position) - pos;
    
  float d = length(l);
  float atten = 1.0 / (gl_LightSource[0].constantAttenuation +
      gl_LightSource[0].linearAttenuation * d +
      gl_LightSource[0].quadraticAttenuation * d * d);
    
  l = normalize(l);
        
  vec3 n = normalize(gl_NormalMatrix * gl_Normal);
  vec3 v = normalize(-pos);
  vec3 h = normalize(l + v);
    
  float nDotL = max(0.0, dot(n, l));
  float nDotH = max(0.0, dot(n, h));
  float power = (nDotL == 0.0) ? 0.0 : pow(nDotH, gl_FrontMaterial.shininess);
    
  vec4 ambient = gl_FrontLightProduct[0].ambient * atten;
  vec4 diffuse = gl_FrontLightProduct[0].diffuse * nDotL * atten;
  vec4 specular = gl_FrontLightProduct[0].specular * power * atten;

  gl_FrontColor = gl_FrontLightModelProduct.sceneColor + ambient + diffuse + specular;
}


