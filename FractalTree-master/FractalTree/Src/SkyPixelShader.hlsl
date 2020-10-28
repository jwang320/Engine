Texture2D shaderTexture : register(t1);
SamplerState SampleType : register(s1);

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

cbuffer PerlinBuffer : register(b3)
{
	float frequency;
	float amplitude;
	float d1;
	float d2;
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
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	const float horizonOffset = 0;
	const float darkLevel = 0;
	const float brightLevel = 1;
	const float skyBrightness = clamp(sunDirection.y, 0, 1);
	if(input.worldPosition.y < horizonOffset)
	{
		return float4(skyBrightness, skyBrightness, skyBrightness, 1);
	}
	else if(input.worldPosition.y > 1 + horizonOffset)
	{
		return float4(0, 0, skyBrightness, 1);
	}
	float4 skyColor = float4(1 + horizonOffset - abs(input.worldPosition.y), (1 + horizonOffset - abs(input.worldPosition.y)), 1, 1) * skyBrightness;
	skyColor.a = 1;
//	return float4(1 - abs(input.worldPosition.y), (1 - abs(input.worldPosition.y))*.25f, abs(input.worldPosition.y), 1);
	return skyColor;//1 - abs(input.worldPosition.y), 1);

}
 