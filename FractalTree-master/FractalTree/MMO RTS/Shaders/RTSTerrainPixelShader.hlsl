Texture2D shaderTexture : register(t1);
Texture2D shaderTexture2 : register(t2);
Texture2D shaderTexture3 : register(t4);
Texture2D shaderTexture4 : register(t5);
Texture2D shaderTexture5 : register(t7);

SamplerState SampleType : register(s1);
/*SamplerState SampleType
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Repeat;
	AddressV = Repeat;
};*/

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix view;
	float4 eyePoint;

	float4 light0Position;
	float4 light0Color;

	float4 sunDirection;
	float4 sunColor;
	float4 cameraUp;
	float4 light1Position;
	float4 light1Color;
	float4 customValueFrame;
};

/*cbuffer OncePerModelBuffer : register(b1)
{
matrix model;
matrix scaleMatrix;
float4 tint;
float4 diffuseTint;
float4 customValueFrame;
};*/

cbuffer PerlinBuffer : register(b3)
{
	float frequency;
	float amplitude;
	float d1;
	float d2;
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

float4 main(TexturedPixelShaderInput input) : SV_TARGET
{
	float4 outputColor = float4(0, 0, 0, 1);//input.ambientColor + input.emissiveColor;// *customValue;
	float4 normal = float4(input.normal[0], input.normal[1], input.normal[2], 0);
	normal.w = 0;
	normal = normalize(normal);

	float4 baseColor = float4(1, 1, 1, 1);
	const float4 greenColor = shaderTexture2.Sample(SampleType, input.texCoord);
	const float4 brownColor = shaderTexture.Sample(SampleType, input.texCoord);
	const float4 blackColor = shaderTexture4.Sample(SampleType, input.texCoord);

	float specChange = 1;
	if (input.normal.y > .8f)
	{
		baseColor *= brownColor*((1 - input.normal.y) / .2f) + greenColor * ((input.normal.y - .8f) / .2f);
		specChange = 0;
	}
	else if (input.normal.y > .6f)
	{
		baseColor *= blackColor*((.8f - input.normal.y) / .2f) + brownColor * ((input.normal.y - .6f) / .2f);
		specChange = (.8f - input.normal.y) / .2f;
	}
	else if (input.normal.y < .6f)
	{
		baseColor *= blackColor;
	}

	baseColor.a = 1;
	return baseColor;
	const float cosFactor = dot(normal, sunDirection.xyz);
	if(cosFactor > 0)
	{
		const float4 diffuseContribution = baseColor * sunColor * cosFactor;
		outputColor+=  diffuseContribution;
		
	}

	outputColor.a = 1;
	return clamp(outputColor, 0, 1);
}
