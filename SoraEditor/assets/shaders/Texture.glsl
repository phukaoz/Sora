#type vertex
#version 420 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aTexIndex;
layout(location = 4) in float aTilingFactor;
layout(location = 5) in int aEntityID;

uniform mat4 uViewProjection;

out vec4 vColor;
out vec2 vTexCoord;
out flat float vTexIndex;
out float vTilingFactor;
out flat int vEntityID;

void main()
{
	vColor = aColor;
	vTexCoord = aTexCoord;
	vTexIndex = aTexIndex;
	vTilingFactor = aTilingFactor;
	vEntityID = aEntityID;
	gl_Position = uViewProjection * vec4(aPosition, 1.0);
}

#type fragment
#version 420 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec4 vColor;
in vec2 vTexCoord;
in flat float vTexIndex;
in float vTilingFactor;
in flat int vEntityID;

uniform sampler2D uTextures[32];

void main()
{
	color = vColor;

	switch (int(vTexIndex)) 
	{
		case  0: color *= texture(uTextures[ 0], vTexCoord * vTilingFactor); break;
		case  1: color *= texture(uTextures[ 1], vTexCoord * vTilingFactor); break;
		case  2: color *= texture(uTextures[ 2], vTexCoord * vTilingFactor); break;
		case  3: color *= texture(uTextures[ 3], vTexCoord * vTilingFactor); break;
		case  4: color *= texture(uTextures[ 4], vTexCoord * vTilingFactor); break;
		case  5: color *= texture(uTextures[ 5], vTexCoord * vTilingFactor); break;
		case  6: color *= texture(uTextures[ 6], vTexCoord * vTilingFactor); break;
		case  7: color *= texture(uTextures[ 7], vTexCoord * vTilingFactor); break;
		case  8: color *= texture(uTextures[ 8], vTexCoord * vTilingFactor); break;
		case  9: color *= texture(uTextures[ 9], vTexCoord * vTilingFactor); break;
		case 10: color *= texture(uTextures[10], vTexCoord * vTilingFactor); break;
		case 11: color *= texture(uTextures[11], vTexCoord * vTilingFactor); break;
		case 12: color *= texture(uTextures[12], vTexCoord * vTilingFactor); break;
		case 13: color *= texture(uTextures[13], vTexCoord * vTilingFactor); break;
		case 14: color *= texture(uTextures[14], vTexCoord * vTilingFactor); break;
		case 15: color *= texture(uTextures[15], vTexCoord * vTilingFactor); break;
		case 16: color *= texture(uTextures[16], vTexCoord * vTilingFactor); break;
		case 17: color *= texture(uTextures[17], vTexCoord * vTilingFactor); break;
		case 18: color *= texture(uTextures[18], vTexCoord * vTilingFactor); break;
		case 19: color *= texture(uTextures[19], vTexCoord * vTilingFactor); break;
		case 20: color *= texture(uTextures[20], vTexCoord * vTilingFactor); break;
		case 21: color *= texture(uTextures[21], vTexCoord * vTilingFactor); break;
		case 22: color *= texture(uTextures[22], vTexCoord * vTilingFactor); break;
		case 23: color *= texture(uTextures[23], vTexCoord * vTilingFactor); break;
		case 24: color *= texture(uTextures[24], vTexCoord * vTilingFactor); break;
		case 25: color *= texture(uTextures[25], vTexCoord * vTilingFactor); break;
		case 26: color *= texture(uTextures[26], vTexCoord * vTilingFactor); break;
		case 27: color *= texture(uTextures[27], vTexCoord * vTilingFactor); break;
		case 28: color *= texture(uTextures[28], vTexCoord * vTilingFactor); break;
		case 29: color *= texture(uTextures[29], vTexCoord * vTilingFactor); break;
		case 30: color *= texture(uTextures[30], vTexCoord * vTilingFactor); break;
		case 31: color *= texture(uTextures[31], vTexCoord * vTilingFactor); break;
	}

	color2 = vEntityID;
}