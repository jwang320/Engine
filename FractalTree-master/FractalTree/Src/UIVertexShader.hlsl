cbuffer OncePerModelBuffer : register(b1)
{
	matrix model;
	float4 tint;
};

struct UIVertexShaderInput
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
};

struct UIVertexShaderOutput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texCoord : TEXOORD0;
};

UIVertexShaderOutput main(UIVertexShaderInput input)
{
	UIVertexShaderOutput output;
	input.position.w = 1;
	output.position = mul(input.position, model);
	output.color = tint;
	output.texCoord = input.texCoord;
	return output;
}