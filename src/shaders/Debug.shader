#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
	TexCoords = aTexCoords;
	gl_Position = vec4(aPosition, 1.0f);
}



#shader fragment
#version 330 core


out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D u_DebugTexture;

void main() {
	FragColor = vec4(texture(u_DebugTexture, TexCoords).rgb, 1.0f);
}