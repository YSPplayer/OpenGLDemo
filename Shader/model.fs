#version 330 core
struct Material {
    vec3 ambient; //环境光照色
    vec3 diffuse; //漫反射颜色，和模型本身的颜色有关
    vec3 specular; //镜面高光的颜色   
    float shininess;//光的散射半径
}; 

struct Light {
    vec3 position;//光源位置
    vec3 ambient;//环境光照分量
    vec3 diffuse;//漫反射分量
    vec3 specular;//镜面反射分量
};

out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
uniform sampler2D ourTexture;
uniform bool useTexture;
uniform bool useLight;
uniform vec3 viewPos;
uniform vec3 defaultObjectColor;
uniform Material material;
uniform Light light;
void main()
{
	// vec3 objectColor = defaultObjectColor;
	// vec3 lightColor = defaultLightColor;
	vec3 objectColor = vec3(0.0, 0.0, 0.0);
	vec3 ambient =  vec3(0.0, 0.0, 0.0);
	vec3 diffuse = vec3(0.0, 0.0, 0.0);
	if(useTexture) {
	   objectColor = texture(ourTexture, TexCoord).rgb;
	} else {
		objectColor = defaultObjectColor;
	}
	if(useLight) {
		//环境光
		if(useTexture) {
		    ambient = light.ambient * objectColor;
		} else {
			ambient = light.ambient * material.ambient;
		}
		//漫反射光
		vec3 lightDir = normalize(light.position - FragPos); 
		vec3 norm = normalize(Normal);
		float diff = max(dot(norm, lightDir), 0.0);
		if(useTexture) {
			diffuse = light.diffuse * (diff * objectColor); 
		} else {
	    	diffuse = light.diffuse * (diff * material.diffuse);     
		}
		//镜面光
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
	    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	    vec3 specular = light.specular * (spec * material.specular);
		objectColor = ambient + diffuse + specular;
	}
	FragColor = vec4(objectColor, 1.0);
} 



