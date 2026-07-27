#version 460 core

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColour;

layout(location = 0) out vec4 outColour;

layout(push_constant) uniform PushConstants
{
    mat4 ViewProjection;
} u_PushConstants;

void main()
{
    gl_Position = u_PushConstants.ViewProjection * vec4(inPosition, 0.0, 1.0);
    
    outColour = inColour;
}
