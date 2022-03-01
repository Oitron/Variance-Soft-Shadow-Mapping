#shader vertex
#version 330 core


layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 u_LightSpaceMatrix;
uniform mat4 u_Model;
out vec4 v_Position;

void main()
{
    gl_Position = u_LightSpaceMatrix * u_Model * vec4(aPosition, 1.0);
    v_Position = gl_Position;
}



#shader fragment
#version 330 core


//layout(location = 0) out vec4 color;
in vec4 v_Position;


void main()
{
    float depth = gl_FragCoord.z;
    float depth_2 = depth * depth;
    //store shadow map and square shadow map in one texture
    // R channel for shadow map, G channel for square shadow map
    gl_FragColor = vec4(depth, depth_2, 0.0, 0.0);
    
}