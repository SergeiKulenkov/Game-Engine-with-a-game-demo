#version 460 core

layout(location = 0) in vec4 inColour;
layout(location = 1) in vec2 inTextureCoord;

layout(location = 0) out vec4 outColour;

layout(set = 0, binding = 0) uniform sampler2D u_Texture;

void main()
{
	outColour = texture(u_Texture, inTextureCoord * 8.f) * inColour;
}
