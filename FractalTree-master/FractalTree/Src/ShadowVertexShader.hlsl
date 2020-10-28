struct SHADOW_PS_INPUT
{
    float4 pos : SV_POSITION;
};
 
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

cbuffer OncePerModelBuffer2 : register(b3)
{
	matrix model;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
struct VertexShaderInput
{
    float4 pos : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
	float2 texCoord : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
SHADOW_PS_INPUT main(VertexShaderInput input)
{
    SHADOW_PS_INPUT output;
    output.pos = mul(float4(input.pos.xyz, 1), model);
	output.pos = mul(output.pos, lightView);
	output.pos = mul(output.pos, lightViewProj);
    return output;
}