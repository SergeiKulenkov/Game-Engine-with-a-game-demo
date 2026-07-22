#version 460 core

layout(location = 0) in vec2 inWorldPosition;
layout(location = 1) in float inThickness;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inVertexPosition;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec2 out_vertexPos;
layout(location = 2) out float out_thickness;

layout(push_constant) uniform PushConstants
{
    mat4 ViewProjection;
} u_PushConstants;

void main()
{
    gl_Position = u_PushConstants.ViewProjection * vec4(inWorldPosition, 0.0, 1.0);
    
    out_vertexPos = inVertexPosition;
    out_color = inColor;
    out_thickness = inThickness;
}
