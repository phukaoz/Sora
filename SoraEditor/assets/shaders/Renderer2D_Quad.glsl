#type vertex
#version 450 core

layout(location = 0) in vec3  a_Position;
layout(location = 1) in vec4  a_Color;
layout(location = 2) in vec2  a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int   a_EntityID;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
};

struct VertexOutput
{
    vec4  Color;
    vec2  TexCoord;
    float TexIndex;
    float TilingFactor;
};

layout (location = 0) out VertexOutput Output;
layout (location = 4) out flat int v_EntityID;

void main()
{
    Output.Color		= a_Color;
    Output.TexCoord     = a_TexCoord;
    Output.TexIndex     = a_TexIndex;
    Output.TilingFactor = a_TilingFactor;
    v_EntityID          = a_EntityID;

    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
    vec4  Color;
    vec2  TexCoord;
    float TexIndex;
    float TilingFactor;
};

layout(location = 0) in VertexOutput Input;
layout(location = 4) in flat int v_EntityID;

layout(binding = 0) uniform sampler2D u_Textures[32];

void main()
{
    o_Color = Input.Color;

    switch (int(Input.TexIndex)) 
    {
        case  0: o_Color *= texture(u_Textures[ 0], Input.TexCoord * Input.TilingFactor); break;
        case  1: o_Color *= texture(u_Textures[ 1], Input.TexCoord * Input.TilingFactor); break;
        case  2: o_Color *= texture(u_Textures[ 2], Input.TexCoord * Input.TilingFactor); break;
        case  3: o_Color *= texture(u_Textures[ 3], Input.TexCoord * Input.TilingFactor); break;
        case  4: o_Color *= texture(u_Textures[ 4], Input.TexCoord * Input.TilingFactor); break;
        case  5: o_Color *= texture(u_Textures[ 5], Input.TexCoord * Input.TilingFactor); break;
        case  6: o_Color *= texture(u_Textures[ 6], Input.TexCoord * Input.TilingFactor); break;
        case  7: o_Color *= texture(u_Textures[ 7], Input.TexCoord * Input.TilingFactor); break;
        case  8: o_Color *= texture(u_Textures[ 8], Input.TexCoord * Input.TilingFactor); break;
        case  9: o_Color *= texture(u_Textures[ 9], Input.TexCoord * Input.TilingFactor); break;
        case 10: o_Color *= texture(u_Textures[10], Input.TexCoord * Input.TilingFactor); break;
        case 11: o_Color *= texture(u_Textures[11], Input.TexCoord * Input.TilingFactor); break;
        case 12: o_Color *= texture(u_Textures[12], Input.TexCoord * Input.TilingFactor); break;
        case 13: o_Color *= texture(u_Textures[13], Input.TexCoord * Input.TilingFactor); break;
        case 14: o_Color *= texture(u_Textures[14], Input.TexCoord * Input.TilingFactor); break;
        case 15: o_Color *= texture(u_Textures[15], Input.TexCoord * Input.TilingFactor); break;
        case 16: o_Color *= texture(u_Textures[16], Input.TexCoord * Input.TilingFactor); break;
        case 17: o_Color *= texture(u_Textures[17], Input.TexCoord * Input.TilingFactor); break;
        case 18: o_Color *= texture(u_Textures[18], Input.TexCoord * Input.TilingFactor); break;
        case 19: o_Color *= texture(u_Textures[19], Input.TexCoord * Input.TilingFactor); break;
        case 20: o_Color *= texture(u_Textures[20], Input.TexCoord * Input.TilingFactor); break;
        case 21: o_Color *= texture(u_Textures[21], Input.TexCoord * Input.TilingFactor); break;
        case 22: o_Color *= texture(u_Textures[22], Input.TexCoord * Input.TilingFactor); break;
        case 23: o_Color *= texture(u_Textures[23], Input.TexCoord * Input.TilingFactor); break;
        case 24: o_Color *= texture(u_Textures[24], Input.TexCoord * Input.TilingFactor); break;
        case 25: o_Color *= texture(u_Textures[25], Input.TexCoord * Input.TilingFactor); break;
        case 26: o_Color *= texture(u_Textures[26], Input.TexCoord * Input.TilingFactor); break;
        case 27: o_Color *= texture(u_Textures[27], Input.TexCoord * Input.TilingFactor); break;
        case 28: o_Color *= texture(u_Textures[28], Input.TexCoord * Input.TilingFactor); break;
        case 29: o_Color *= texture(u_Textures[29], Input.TexCoord * Input.TilingFactor); break;
        case 30: o_Color *= texture(u_Textures[30], Input.TexCoord * Input.TilingFactor); break;
        case 31: o_Color *= texture(u_Textures[31], Input.TexCoord * Input.TilingFactor); break;
    }

    if (o_Color.a == 0.0)
        discard;
    
    o_EntityID = v_EntityID;
}