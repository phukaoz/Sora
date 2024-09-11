#type vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aTexIndex;
layout(location = 4) in float aTilingFactor;
layout(location = 5) in int aEntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 uViewProjection;
};

struct VertexOutput
{
	vec4  Color;
	vec2  TexCoord;
	float TexIndex;
	float TilingFactor;
};

layout (location = 0) out VertexOutput Output;
layout (location = 4) out flat int vEntityID;

void main()
{
	Output.Color = aColor;
	Output.TexCoord = aTexCoord;
	Output.TexIndex = aTexIndex;
	Output.TilingFactor = aTilingFactor;
	vEntityID = aEntityID;

	gl_Position = uViewProjection * vec4(aPosition, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

struct VertexOutput
{
	vec4  Color;
	vec2  TexCoord;
	float TexIndex;
	float TilingFactor;
};

layout(location = 0) in VertexOutput Input;
layout(location = 4) in flat int vEntityID;

layout(binding = 0) uniform sampler2D uTextures[32];

void main()
{
	color = Input.Color;

	switch (int(Input.TexIndex)) 
	{
		case  0: color *= texture(uTextures[ 0], Input.TexCoord * Input.TilingFactor); break;
		case  1: color *= texture(uTextures[ 1], Input.TexCoord * Input.TilingFactor); break;
		case  2: color *= texture(uTextures[ 2], Input.TexCoord * Input.TilingFactor); break;
		case  3: color *= texture(uTextures[ 3], Input.TexCoord * Input.TilingFactor); break;
		case  4: color *= texture(uTextures[ 4], Input.TexCoord * Input.TilingFactor); break;
		case  5: color *= texture(uTextures[ 5], Input.TexCoord * Input.TilingFactor); break;
		case  6: color *= texture(uTextures[ 6], Input.TexCoord * Input.TilingFactor); break;
		case  7: color *= texture(uTextures[ 7], Input.TexCoord * Input.TilingFactor); break;
		case  8: color *= texture(uTextures[ 8], Input.TexCoord * Input.TilingFactor); break;
		case  9: color *= texture(uTextures[ 9], Input.TexCoord * Input.TilingFactor); break;
		case 10: color *= texture(uTextures[10], Input.TexCoord * Input.TilingFactor); break;
		case 11: color *= texture(uTextures[11], Input.TexCoord * Input.TilingFactor); break;
		case 12: color *= texture(uTextures[12], Input.TexCoord * Input.TilingFactor); break;
		case 13: color *= texture(uTextures[13], Input.TexCoord * Input.TilingFactor); break;
		case 14: color *= texture(uTextures[14], Input.TexCoord * Input.TilingFactor); break;
		case 15: color *= texture(uTextures[15], Input.TexCoord * Input.TilingFactor); break;
		case 16: color *= texture(uTextures[16], Input.TexCoord * Input.TilingFactor); break;
		case 17: color *= texture(uTextures[17], Input.TexCoord * Input.TilingFactor); break;
		case 18: color *= texture(uTextures[18], Input.TexCoord * Input.TilingFactor); break;
		case 19: color *= texture(uTextures[19], Input.TexCoord * Input.TilingFactor); break;
		case 20: color *= texture(uTextures[20], Input.TexCoord * Input.TilingFactor); break;
		case 21: color *= texture(uTextures[21], Input.TexCoord * Input.TilingFactor); break;
		case 22: color *= texture(uTextures[22], Input.TexCoord * Input.TilingFactor); break;
		case 23: color *= texture(uTextures[23], Input.TexCoord * Input.TilingFactor); break;
		case 24: color *= texture(uTextures[24], Input.TexCoord * Input.TilingFactor); break;
		case 25: color *= texture(uTextures[25], Input.TexCoord * Input.TilingFactor); break;
		case 26: color *= texture(uTextures[26], Input.TexCoord * Input.TilingFactor); break;
		case 27: color *= texture(uTextures[27], Input.TexCoord * Input.TilingFactor); break;
		case 28: color *= texture(uTextures[28], Input.TexCoord * Input.TilingFactor); break;
		case 29: color *= texture(uTextures[29], Input.TexCoord * Input.TilingFactor); break;
		case 30: color *= texture(uTextures[30], Input.TexCoord * Input.TilingFactor); break;
		case 31: color *= texture(uTextures[31], Input.TexCoord * Input.TilingFactor); break;
	}

	color2 = vEntityID;
}