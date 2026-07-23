#version 460 core

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTextureCoord;

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_TextureCoord;

layout(push_constant) uniform PushConstants
{
    mat4 ViewProjection;
} u_PushConstants;

void main()
{
    gl_Position = u_PushConstants.ViewProjection * vec4(inPosition, 0.0, 1.0);
    //out_color = inColor;
    out_TextureCoord = inTextureCoord;
}
