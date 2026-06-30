#version 460 core

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inPosition;

layout(location = 0) out vec4 out_color;

void main()
{
	float thickness = 0.005;
    float fade = 0.005;
    
    float distance = 1.0 - length(gl_FragCoord);
    float alpha = smoothstep(0.0, fade, distance);
    alpha *= smoothstep(thickness + fade, thickness, distance);

    out_color.rgb = inColor;
    out_color.a *= alpha;
}