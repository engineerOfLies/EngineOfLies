
uniform sampler2D colorMap;

varying vec3 normal; 
varying vec3 position;


vec4 lightSource( vec3 N, vec3 V, gl_LightSourceParameters light )
{
	vec3  H;
	float d = length( light.position.xyz - V );
	vec3  L = normalize( light.position.xyz - V );


	H = normalize( L - V.xyz );

        float atten = 1.0 / (light.constantAttenuation +
            light.linearAttenuation * d +
            light.quadraticAttenuation * d * d);

	float NdotL = max( 0.0, dot( N,L ) );
	float NdotH = max( 0.0, dot( N,H ) );

	float Idiff = NdotL;
	float Ispec = pow( NdotH, gl_FrontMaterial.shininess );



	return 
            (gl_FrontMaterial.ambient  * light.ambient * atten) +
            (gl_FrontMaterial.diffuse  * light.diffuse  * Idiff * atten)  +
            (gl_FrontMaterial.specular * light.specular * Ispec* atten) ;
}



vec4 lighting( void )
{
	vec3 N = normalize( normal );
        vec4 color;
        
        color = gl_FrontMaterial.emission +
            gl_FrontMaterial.ambient * gl_LightModel.ambient +
            lightSource( N, position, gl_LightSource[0] );
        return
            color * texture2D(colorMap, gl_TexCoord[0].st);
}



void main( void )
{
  gl_FragColor = lighting();
}

