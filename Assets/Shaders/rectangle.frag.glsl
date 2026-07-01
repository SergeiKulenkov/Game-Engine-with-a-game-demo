#version 460 core

layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 inTextureCoord;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D u_Texture;

void main()
{
	vec4 textureColor = texture(u_Texture, inTextureCoord);

	//out_color = vec4(in_color * textureColor.rgb, 1.0f);
	out_color = vec4(textureColor.rgb, 1.0f);
}
