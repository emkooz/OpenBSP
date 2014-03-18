#version 150 core

out vec4 outColor;
in vec3 normalColor;

void main()
{
	outColor = vec4 (normalColor.x, normalColor.y, normalColor.z, 1.0);
}