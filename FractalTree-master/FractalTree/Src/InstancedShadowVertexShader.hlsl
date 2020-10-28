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

struct InstancedVertexShaderIn
{
    float4 pos : POSITION;
    float4 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    row_major float4x4 mTransform : mTransform;
};

struct TexturedPixelShaderInput
{
    float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
	float4 worldPosition : WORLD_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 shadowCoord : TEXCOORD1;
	float specularExponent : NIL;
};

TexturedPixelShaderInput main(InstancedVertexShaderIn input)
{
	TexturedPixelShaderInput output;

	output.pos = mul(float4(input.pos.xyz, 1), input.mTransform);
    output.pos = mul(output.pos, model);
	output.worldPosition = output.pos;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);

    // Pass through the color without modification.
	output.texCoord = input.texCoord;
	output.normal = normalize(mul(input.normal, input.mTransform));
	output.normal = normalize(mul(output.normal, model));

	output.tangent = normalize(mul(float4(0, 1, 0, 0), input.mTransform));
	output.tangent = normalize(mul(output.tangent, model));

	output.binormal = float4(cross(output.normal.xyz, output.tangent.xyz), 0);

	output.specularExponent = 55;

	output.shadowCoord = mul(output.worldPosition, lightView);
	output.shadowCoord = mul(output.shadowCoord, lightViewProj);

	return output;
}