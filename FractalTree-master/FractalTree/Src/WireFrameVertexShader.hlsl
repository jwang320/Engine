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
	float4 diffuseTint;
	float4 customValue;
};

cbuffer OncePerResizeBuffer : register(b2)
{
    matrix projection;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

struct VertexShaderInput
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	output.color = input.color;
	
	float4 pos = input.pos;
	pos.w = 1;
	pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);
    output.pos = pos;


	return output;
}