Texture2D shaderTexture : register(t1);
SamplerState SampleType : register(s1);
Texture2D bumpTexture : register(t6);

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

/*cbuffer OncePerModelBuffer : register(b1)
{
matrix model;
matrix scaleMatrix;
float4 tint;
float4 diffuseTint;
float4 customValueFrame;
};*/
#define _USE_FOG

cbuffer PerlinBuffer : register(b3)
{
	float4 textureRepeatCount;
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

float4 main(PixelShaderInput input) : SV_TARGET
{
	const float4 fogColor = float4(clamp(sunDirection.y, 0, 1), clamp(sunDirection.y, 0, 1), clamp(sunDirection.y, 0, 1), 1);
const float fogDist = 100000;
const float cameraDistance = distance(eyePoint.xyz, input.worldPosition);
//if(cameraDistance > fogDist)
{
	//	return fogColor;
}

#ifdef _USE_FOG
const float fogDensity = .00001f * textureRepeatCount.y;//.00012f;//.00006f;
														//float fogFL = (fogDist - cameraDistance) / fogDist;
float fogFE = pow(1.0 / 2.71828, cameraDistance * fogDensity * cameraDistance * fogDensity);//cameraDistance * fogDensity);
if (fogFE < .015f)
{
	return fogColor;
}
#endif
	//return float4(customValueFrame.x, customValueFrame.y, customValueFrame.z, 1);

	//return float4(1, 1, 1, 1);
	if (input.emissiveColor.x + input.emissiveColor.y + input.emissiveColor.z > 0)
	{
		//input.emissiveColor.w = input.color.w;
		//return input.emissiveColor;
		//input.color.xyz *= .25f;
	}

////return customValue;
//return input.emissiveColor;
float4 outputColor = input.ambientColor + input.emissiveColor;// *customValue;
float4 normal = float4(input.normal[0], input.normal[1], input.normal[2], input.normal[3]);
float3 bumpMap = bumpTexture.Sample(SampleType, input.texCoord * 100 + customValueFrame.xy).xyz;
bumpMap = (bumpMap * 2.0f) - 1.0f;
input.normal = normalize(input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal);
normal.w = 0;
input.normal.x *= sin(customValueFrame.z * 3);// +cos(customValueFrame.z);
input.normal.z *= sin(customValueFrame.z * 5);// +cos(customValueFrame.z);
//input.normal.y *= abs(sin(customValueFrame.z) * 5);// +cos(customValueFrame.z);
normal = normalize(normal);

{

	{
		const float cosFactor = dot(input.normal, sunDirection.xyz);
		if(cosFactor > 0)
		{
		const float4 diffuseContribution = input.color * sunColor * cosFactor;
		outputColor+=  diffuseContribution;
		const float3 r = normalize(reflect(sunDirection.xyz, input.normal));
		const float num = dot(r, normalize(input.worldPosition - eyePoint.xyz));

		if (num > 0)
		{
			const float s = pow(num, 55);
			outputColor += sunColor/*float4(.7f, .7f, .7f, 0)*/ * s * .25f;
		}
		}
	}

}

/*{
const float cosFactor = dot(normal, sunDirection.xyz);
if(cosFactor > 0)
{
const float4 diffuseContribution = input.color * sunColor * cosFactor;
outputColor+=  diffuseContribution;
const float3 r = normalize(reflect(sunDirection.xyz, normal));
const float num = dot(r, normalize(input.worldPosition - eyePoint.xyz));

if(num > 0)
{
const float s = pow(num, 55);
outputColor+= sunColor/ *float4(.7f, .7f, .7f, 0)* / * s * .25f;
}
}
}*/
#ifdef _USE_FOG

const float4 newColor = fogFE * outputColor + (1 - fogFE) * fogColor;
//outputColor+= 1.0 / 2.71828 * (cameraDistance * 2) * (cameraDistance * 2) * float4(1, 1, 1, 1);
outputColor = newColor;
#endif

outputColor.a = input.color.a;
return clamp(outputColor, 0, 1);
}
