#type vertex
#version 450 core

layout(location = 0) in vec3 aWorldPosition;
layout(location = 1) in vec3 aLocalPosition;
layout(location = 2) in vec4 aColor;
layout(location = 3) in float aThickness;
layout(location = 4) in float aFade;
layout(location = 5) in int aEntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 uViewProjection;
};

struct VertexOutput
{
	vec4 Color;
	vec3 LocalPosition;
	float Thickness;
	float Fade;
};

layout (location = 0) out VertexOutput Output;
layout (location = 4) out flat int vEntityID;

void main()
{
	Output.LocalPosition = aLocalPosition;
	Output.Color = aColor;
	Output.Thickness = aThickness;
	Output.Fade = aFade;
	vEntityID = aEntityID;

	gl_Position = uViewProjection * vec4(aWorldPosition, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

struct VertexOutput
{
	vec4 Color;
	vec3 LocalPosition;
	float Thickness;
	float Fade;
};

layout(location = 0) in VertexOutput Input;
layout(location = 4) in flat int vEntityID;

void main()
{
	float distance = 1.0 - length(Input.LocalPosition);
    float circle = smoothstep(0.0, Input.Fade, distance);
    circle *= smoothstep(Input.Thickness + Input.Fade, Input.Thickness, distance);

	if (circle == 0.0)
		discard;

    oColor = Input.Color;
	oColor.a *= circle;
	

	oEntityID = vEntityID;
}