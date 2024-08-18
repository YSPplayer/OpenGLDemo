	#version 330 core
	layout (location = 0) in vec3 aPos; 
	layout (location = 1) in vec2 aTexCoord;
	layout (location = 2) in vec3 aNormal;
	layout (location = 3) in vec3 aTangent;
	layout (location = 4) in vec3 aBitangent;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
	uniform mat3 normalMatrix;
	uniform bool useNormalTexture;//是否启用法线贴图
	uniform vec3 viewPos;
	uniform vec3 lightPos;
	out vec2 TexCoord;
	out vec3 Normal;
	out vec3 FragPos;
	out vec3 tangentLightPos;
	out	vec3 tangentViewPos;
	out	vec3 tangentFragPos;
	void main()
	{
        FragPos = vec3(model * vec4(aPos, 1.0));
		gl_Position = projection * view * vec4(FragPos, 1.0);
		TexCoord = aTexCoord;
		Normal = normalMatrix * aNormal;
		if(useNormalTexture) { //启用法线贴图
			vec3 T = normalize(normalMatrix * aTangent);
			vec3 B = normalize(normalMatrix * aBitangent);
			vec3 N = normalize(Normal); 
			mat3 TBN = transpose(mat3(T, B, N));  
			tangentLightPos = TBN * lightPos;
			tangentViewPos  = TBN * viewPos;
			tangentFragPos  = TBN * FragPos;
		} 
	}