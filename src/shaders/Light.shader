#shader vertex
#version 330 core 

layout(location = 0) in vec3 aPosition; 

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
	mat4 MVP = u_Projection * u_View * u_Model;
	gl_Position = MVP * vec4(aPosition, 1.0);
};





#shader fragment
#version 330 core 

layout(location = 0) out vec4 color; 

uniform vec3 u_LightColor;

void main() {
	color = vec4(u_LightColor, 1.0f);
};