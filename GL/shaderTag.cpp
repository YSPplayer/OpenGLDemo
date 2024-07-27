#include "shaderTag.h"
namespace GL {
	//从索引位置0开始，每次截取3个顶点数据作为对象 gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	std::string vertexShader = R"glsl(
		#version 330 core
		layout (location = 0) in vec3 aPos; 
		layout (location = 1) in vec2 aTexCoord;
		layout (location = 2) in vec3 aNormal;
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;
		uniform mat3 normalMatrix;
		out vec2 TexCoord;
		out vec3 Normal;
		out vec3 FragPos;
		void main()
		{
			gl_Position = projection * view * model * vec4(aPos, 1.0);
			FragPos = vec3(model * vec4(aPos, 1.0));
			//gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
			TexCoord = aTexCoord;
			Normal = normalMatrix * aNormal;
		}
	)glsl";
	std::string colorLightShader = R"glsl(	
		#version 330 core
		out vec4 FragColor;
		void main()
		{
			FragColor = vec4(1.0); 
		}
	)glsl";
	std::string colorShader = R"glsl(
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
	uniform float reflectivity; // 反射度因子，范围 0-256
	void main()
	{
		vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
		vec3 objectColor = vec3(1.0f, 0.5f, 0.2f);
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

			float spec = pow(max(dot(viewDir, reflectDir), 0.0),reflectivity);
			vec3 specular = specularStrength * spec * lightColor;
			objectColor = (ambient + diffuse + specular) * objectColor;
		}
		FragColor = vec4(objectColor, 1.0f);
	} 
	)glsl";
}