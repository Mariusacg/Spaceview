#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;

in vec4 fragPosLightSpace;

out vec4 fColor;

//lighting
//directional light
uniform	vec3 lightDir;
uniform	vec3 lightColor;

//pointlight

uniform vec3 pointLightPos;
uniform vec3 pointLightdirection;
uniform float amplifier;

float distance;
float attenuation;

float constant = 1.0f;
float linear = 0.09f;
float quadratic = 0.032f;

//spotlight
//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//shadow map
uniform sampler2D shadowMap;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

float shadow;

float bias = 0.005f;

float computeShadow() 
{
	//perform perspective divide
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	
	if (normalizedCoords.z > 1.0f)
		return 0.0f;

	float currentDepth = normalizedCoords.z;

	//check if current pos in shadow
	return (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
}

void computeLightPositional()
{
	distance = length(pointLightPos - fPosEye.xyz);

	attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance)) * amplifier;

	vec3 cameraPosEye = vec3(0.0f);
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(pointLightdirection);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
}
void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

void main() 
{
	computeLightComponents();
	
	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	//vec3 color = min((ambient + diffuse) + specular, 1.0f);
    	//fColor = vec4(color, 1.0f);


	shadow = computeShadow();
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);

	computeLightPositional();

	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	vec3 colorpoint = min((ambient + diffuse) + specular, 1.0f);

	fColor = vec4(max(color, colorpoint), 1.0f);
}
