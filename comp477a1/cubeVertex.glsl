
#version 330 core
  
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoordinate;

out vec2 TexCoord;

uniform mat4 model = mat4(1.0f);
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(aPosition, 1.0f);
	TexCoord = aTexCoordinate;
}