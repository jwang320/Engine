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
	float4 light1Position;
	float4 light1Color;
	float4 customValueFrame;
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

struct VertexShaderInput
{
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
	float2 texCoord : TEXCOORD0;
};

struct TexturedPixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 worldPosition : WORLD_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 shadowCoord : TEXCOORD1;
};

TexturedPixelShaderInput main(VertexShaderInput input)
{
	TexturedPixelShaderInput output;
	// Transform the vertex position into projected space.
	output.pos = mul(float4(input.pos.xyz, 1), model);
	output.worldPosition = output.pos.xyz;
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	// Pass through the color without modification.
	output.texCoord = input.texCoord;
	output.normal = normalize(mul(input.normal, model)).xyz;
	output.tangent = normalize(mul(input.tangent, model)).xyz;
	output.binormal = normalize(mul(input.binormal, model)).xyz;

	output.shadowCoord = mul(float4(output.worldPosition, 1), lightView);
	output.shadowCoord = mul(output.shadowCoord, lightViewProj);

	//	output.light0Pos = normalize(light0Position - output.worldPosition);
	return output;
}
