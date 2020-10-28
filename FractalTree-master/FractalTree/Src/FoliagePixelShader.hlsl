Texture2D shaderTexture : register(t1);
Texture2D bumpTexture : register(t6);
SamplerState SampleType : register(s1);
Texture2D leafBump : register(t9);
Texture2D shadowMap : register(t10);
#define _USE_SHADOWS
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

struct TexturedPixelShaderInput
{
    float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
	float4 worldPosition : WORLD_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 shadowCoord : TEXCOORD1;
};

float4 main(TexturedPixelShaderInput input) : SV_TARGET
{
	const float4 baseColor = shaderTexture.Sample(SampleType, input.texCoord);
	
	if(baseColor.a <.5f)
	{
		return float4(0, 0, 0, 0);
	}
	
	float4 outputColor = baseColor* max(sunDirection.y, .4f) *.5f;//float4(0, 0, 0, 0);
		
	input.normal = normalize(input.normal);//float4(input.normal[0], input.normal[1], input.normal[2], 0));
	
#ifdef _USE_SHADOWS
	input.shadowCoord.xyz /= input.shadowCoord.w;
	//transform clip space coords to texture space coords (-1:1 to 0:1)
	input.shadowCoord.x = input.shadowCoord.x/2 + 0.5;
	input.shadowCoord.y = input.shadowCoord.y/-2 + 0.5;

	float shadowMapDepth = shadowMap.Sample(SampleType, input.shadowCoord.xy).r;
		
	if(input.shadowCoord.x < -1.0f || input.shadowCoord.x > 1.0f ||
		input.shadowCoord.y < -1.0f || input.shadowCoord.y > 1.0f ||
		input.shadowCoord.z < 0.0f  || input.shadowCoord.z > 1.0f)
	{
		shadowMapDepth = 10000000;
	}

	if(shadowMapDepth >= input.shadowCoord.z && sunDirection.y > 0)
	{
		//input.normal = float4(0, 1, 0, 0);
		const float cosFactor = dot(input.normal, sunDirection);
		if(cosFactor > 0)
		{
			const float4 diffuseContribution = baseColor * sunColor * cosFactor;
			outputColor+=  diffuseContribution;
			/*const float4 r = normalize(reflect(sunDirection, input.normal));
			const float num = dot(r, normalize(input.worldPosition - eyePoint));

			if(num > 0)
			{
				const float s = pow(num, input.specularExponent);
				outputColor+= float4(.7f, .7f, .7f, 0) * s;
			}*/
		}
	}	
#else
	const float cosFactor = dot(input.normal, sunDirection);
		if(cosFactor > 0)
		{
			const float4 diffuseContribution = baseColor * sunColor * cosFactor;
			outputColor+=  diffuseContribution;
			/*const float4 r = normalize(reflect(sunDirection, input.normal));
			const float num = dot(r, normalize(input.worldPosition - eyePoint));

			if(num > 0)
			{
				const float s = pow(num, input.specularExponent);
				outputColor+= float4(.7f, .7f, .7f, 0) * s;
			}*/
		}
#endif
	
	const float len = distance(input.worldPosition.xyz, light0Position.xyz);

   if(len < 5000)
    {
		float3 lightDirection = light0Position.xyz - input.worldPosition.xyz;
			lightDirection = normalize(lightDirection);
		const float cosFactor = dot(input.normal.xyz, lightDirection);

		if (cosFactor > 0)
		{
			const float lenFactor = min(1 / (.0001f * len*len), 1);
			const float4 diffuseContribution = baseColor * sunColor/*light0Color*/ * cosFactor * lenFactor;
			outputColor += diffuseContribution;
			const float3 r = normalize(reflect(lightDirection, input.normal.xyz));
			const float s = pow(max(dot(r, normalize(input.worldPosition.xyz - eyePoint.xyz)), 0.0), 55);
			outputColor += sunColor/*float4(.7f, .7f, .7f, 0)*/ * s * lenFactor;
		}
	}
	
	/*const float cameraDistance = distance(eyePoint, input.worldPosition);
	if(cameraDistance < 1300)
	{
		const float4 lightDirection = normalize(eyePoint - input.worldPosition);
		const float cosFactor = dot(input.normal, lightDirection);

		if(cosFactor > 0)
		{
			const float lenFactor = min(1 / (.0001f * cameraDistance*cameraDistance), 1);
			const float4 diffuseContribution = (baseColor) * sunColor * cosFactor * lenFactor;
			outputColor+=  diffuseContribution;
		}
		
	}*/

	outputColor.a = 1;
	return outputColor;
}