cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix view;
	float3 eyePoint;

	float3 light0Position;
	float3 light0Color;

	float3 sunDirection;
	float3 sunColor;
	float3 cameraUp;
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

struct InstancedVertexShaderIn
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

	row_major float4x4 mTransform : mTransform;
	float4 diffuseTint : COLOR4;
	float4 emissiveTint : COLOR5;
	float specularExponentTint : NILB;
};

struct PixelShaderInput
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
	float4 customValue : COLOR4;
};

PixelShaderInput main(InstancedVertexShaderIn input)
{
	PixelShaderInput output;

	output.pos = mul(float4(input.pos.xyz, 1), input.mTransform);
	output.pos = mul(output.pos, model);
	output.worldPosition = output.pos;
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	// Pass through the color without modification.
	output.texCoord = input.texCoord;
	output.normal = normalize(mul(input.normal, input.mTransform));
	output.normal = normalize(mul(output.normal, model));

	output.binormal = float4(0, 1, 0, 0);
	output.tangent = float4(0, 1, 0, 0);
	output.color = input.diffuseTint;
	output.emissiveColor = input.emissiveTint;
	output.specularColor = float4(1, 1, 1, 1);
	output.ambientColor = float4(0, 0, 0, 0);
	output.customValue = customValue;
	//output.tangent = normalize(mul(float4(0, 1, 0, 0), input.mTransform));
	//output.tangent = normalize(mul(output.tangent, model));

	//output.binormal = float4(cross(output.normal.xyz, output.tangent.xyz), 0);

	output.specularExponent = input.specularExponentTint;

	//output.shadowCoord = mul(output.worldPosition, lightView);
	//output.shadowCoord = mul(output.shadowCoord, lightViewProj);

	return output;
}