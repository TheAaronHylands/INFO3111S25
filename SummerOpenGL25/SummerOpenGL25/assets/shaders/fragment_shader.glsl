#version 420

in vec4 vertColor;
in vec4 vertNormal;
in vec4 vertWorldPosition;

out vec4 pixelColour;

vec4 calculateLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular);
							
struct sLight
{
	vec4 position;	// xyz, ignoring w (4th parameter)	
	vec4 diffuse;	
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights, ignoring w (4th parameter)	
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBEROFLIGHTS = 10;
sLight theLights[NUMBEROFLIGHTS];

void main()
{
    //gl_FragColor = vec4(vertColor, 1.0);
	pixelColour = vec4(vertColor);
	
	
	pixelColour.rgb = vertNormal.xyz;
	
	
};

