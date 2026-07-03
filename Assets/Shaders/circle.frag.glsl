#version 460 core

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inPosition;

layout(location = 0) out vec4 out_color;

void main()
{
	float thickness = 0.05;
    float fade = 0.005;
    
    float distance = 1.0 - length(inPosition);
    float alpha = smoothstep(0.0, fade, distance);
    alpha *= smoothstep(thickness + fade, thickness, distance);

    if (alpha == 0.0)
        discard;

    out_color = vec4(inColor, 1.0);
    out_color.a *= alpha;
}