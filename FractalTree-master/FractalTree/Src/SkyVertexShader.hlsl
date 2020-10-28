cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix view;
	float4 eyePoint;

	float4 light0Position;
	float4 light0Color;

	float4 sunDirection;
	float4 sunColor;
	float4 cameraUp;
	matrix lightViewProj;
	matrix lightView;
};

cbuffer OncePerModelBuffer : register(b1)
{
	matrix model;
	matrix scaleMatrix;
	float4 tint;
};

cbuffer OncePerResizeBuffer : register(b2)
{
    matrix projection;
};

struct VertexShaderInput
{
    float4 pos : POSITION;
    float4 color : COLOR0;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
    float4 emissiveColor : COLOR1;
	float4 specularColor : COLOR2;
	float4 ambientColor : COLOR3;
	float2 texCoord : TEXCOORD0;
	float specularExponent : NIL;
};

struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
	float4 worldPosition : WORLD_POSITION;
    float4 emissiveColor : COLOR1;
	float4 specularColor : COLOR2;
	float4 ambientColor : COLOR3;
	float2 texCoord : TEXCOORD0;
	float specularExponent : NIL;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float4 pos = input.pos;
	//pos.w = 0;
	pos.w = 1;
	//pos[3] = 1;
    // Transform the vertex position into projected space.
	output.worldPosition = pos;
    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);
    output.pos = pos;

    // Pass through the color without modification.
    output.color = input.color;
	input.normal.w = 0;
	output.texCoord = input.texCoord;
	output.normal = normalize(mul(input.normal, model));
	output.tangent = normalize(mul(input.tangent, model));
	output.binormal = normalize(mul(input.binormal, model));
	output.emissiveColor = input.emissiveColor + tint;
	output.specularColor = input.specularColor;
	output.specularExponent = input.specularExponent;
	output.ambientColor = input.ambientColor;
	//output.color.a = .25f;
    return output;
}
