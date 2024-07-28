#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
uniform sampler2D ourTexture;
uniform float ambientStrength;	
uniform	float specularStrength;
uniform bool useTexture;
uniform bool useLight;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 defaultObjectColor;
uniform vec3 defaultLightColor;
uniform float reflectivity; // 反射度因子，范围 0-256
void main()
{
	vec3 objectColor = defaultObjectColor;
	vec3 lightColor = defaultLightColor;

	if(useTexture) {
	   objectColor = texture(ourTexture, TexCoord).rgb;
	}
	if(useLight) {
		//环境光
		vec3 ambient = ambientStrength * lightColor;

		//漫反射光
		vec3 lightDir = normalize(lightPos - FragPos); 
		vec3 norm = normalize(Normal);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColor;       
		
		//镜面光
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
//reflectivity
		float spec = pow(max(dot(viewDir, reflectDir), 0.0),256);
		vec3 specular = specularStrength * spec * lightColor;
		objectColor = (ambient + diffuse + specular) * objectColor;
	}
	FragColor = vec4(objectColor, 1.0f);
} 



