#version 330 core
struct Material {
    vec3 ambient; //环境光照色
    vec3 diffuse; //漫反射颜色，和模型本身的颜色有关
    vec3 specular; //镜面高光的颜色   
    float shininess;//光的散射半径
}; 

struct Light {
    vec3 position;//光源位置
	vec3 direction;//平行光位置
    vec3 ambient;//环境光照分量
    vec3 diffuse;//漫反射分量
    vec3 specular;//镜面反射分量

	float constant;//非线性光源常数项1
    float linear;//非线性光源常数项2
    float quadratic;//非线性光源常数项3

	float cutOff;//聚光角
	float outerCutOff;//聚光边缘角平缓
};

out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
uniform sampler2D defaultTexture;
uniform sampler2D specularTexture;
uniform bool useTexture;
uniform bool useLight;
uniform vec3 viewPos;
uniform vec3 defaultObjectColor;
uniform Material material;
uniform Light light;
uniform int lightType;
void main() {
	vec3 objectColor = vec3(0.0, 0.0, 0.0);
	vec3 ambient =  vec3(0.0, 0.0, 0.0);
	vec3 diffuse = vec3(0.0, 0.0, 0.0);
	vec3 specular = vec3(0.0, 0.0, 0.0);
	if(useTexture) {
	   objectColor = texture(defaultTexture, TexCoord).rgb;
	} else {
		objectColor = defaultObjectColor;
	}
	if(useLight) {
		vec3 lightDir = vec3(0.0, 0.0, 0.0);
		vec3 norm = normalize(Normal);
		float attenuation = 0.0;
		float distance = 0.0;
		if(lightType == 0) { //平行光
			lightDir = normalize(-light.direction);
		} else if(lightType == 1) { //线性点光源
			lightDir = normalize(light.position - FragPos); 
		} else if(lightType == 2 || lightType == 3) { //非线性光源 非线性聚光
			lightDir = normalize(light.position - FragPos);
			distance = length(light.position - FragPos);
			attenuation = 1.0 / (light.constant + light.linear * distance + 
				light.quadratic * (distance * distance));   
		} 
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		if(useTexture) {
			ambient = light.ambient * objectColor;//环境光
			diffuse = light.diffuse * (diff * objectColor);//漫反射光 
			specular = light.specular * (spec * texture(specularTexture, TexCoord).rgb);//镜面光  
		} else {
			ambient = light.ambient * material.ambient;//环境光
			diffuse = light.diffuse * (diff * material.diffuse);//漫反射光     
			specular = light.specular * (spec * material.specular);//镜面光   
		}
		if(lightType == 3) {//聚光调节,为了让聚光的边缘看起来变化的更平缓
			float theta = dot(lightDir, normalize(-light.direction)); 
			float epsilon = (light.cutOff - light.outerCutOff);
			float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
			diffuse  *= intensity;
    		specular *= intensity;
		}
		if(lightType == 2 || lightType == 3) { //非线性光源调节光线和聚光调节光线
			ambient *= attenuation;  
			diffuse *= attenuation;
			specular *= attenuation;   
		} 
		objectColor = ambient + diffuse + specular;
	}
	FragColor = vec4(objectColor, 1.0);
} 



