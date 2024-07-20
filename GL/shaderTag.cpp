#include "shaderTag.h"
namespace GL {
	//从索引位置0开始，每次截取3个顶点数据作为对象 gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	std::string vertexShader = R"glsl(
		#version 330 core
		layout (location = 0) in vec3 aPos; 
		layout (location = 1) in vec2 aTexCoord;
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;
		out vec2 TexCoord;
		void main()
		{
			gl_Position = projection * view * model * vec4(aPos, 1.0);
			//gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
			TexCoord = aTexCoord;
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
	uniform sampler2D ourTexture;
	uniform bool useTexture;
	void main()
	{
		if(useTexture) {
			FragColor = texture(ourTexture, TexCoord);
		} else {
			// * vec3(1.0f, 1.0f, 1.0f)
			FragColor = vec4(vec3(1.0f, 0.5f, 0.31f) * vec3(1.0f, 1.0f, 1.0f), 1.0f);
		}
	} 
	)glsl";
}