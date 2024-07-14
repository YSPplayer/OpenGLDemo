#pragma once
#include <string>
namespace GL {
	//������λ��0��ʼ��ÿ�ν�ȡ3������������Ϊ���� gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	std::string vertexShader = R"glsl(
		#version 330 core
		layout (location = 0) in vec3 aPos; 
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;
		void main()
		{
			gl_Position = projection * view * model * vec4(aPos, 1.0);
			//gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
		}
	)glsl";
	
	std::string colorShader =  R"glsl(
	#version 330 core
	out vec4 FragColor;
	void main()
	{
		FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	} 
	)glsl";
}