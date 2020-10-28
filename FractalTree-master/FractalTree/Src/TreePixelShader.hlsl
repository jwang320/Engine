Texture2D shaderTexture : register(t1);
Texture2D bumpTexture : register(t6);
Texture2D shadowMap : register(t10);
SamplerState SampleType : register(s1);
//#define  _USE_SHADOWS
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

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 worldPosition : WORLD_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 shadowCoord : TEXCOORD1;
};

SamplerComparisonState cmpSampler : register(s2)
{
   // sampler state
   Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
   AddressU = MIRROR;
   AddressV = MIRROR;
 
   // sampler comparison state
   ComparisonFunc = LESS_EQUAL;
};
float2 texOffset( int u, int v )
{
    return float2( u * 1.0f/1024, v * 1.0f/1024);
}

#define DD

float4 main(PixelShaderInput input) : SV_TARGET
{
	const float4 baseColor = shaderTexture.Sample(SampleType, input.texCoord);
	float4 outputColor = float4((baseColor*.2f).xyz, 1);//float4(0, 0, 0, 1);

	input.normal = normalize(input.normal);
	float3 bumpMap = bumpTexture.Sample(SampleType, input.texCoord * 2).xyz;
	bumpMap = (bumpMap * 2.0f) - 1.0f;
	const float3 bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
	input.normal = normalize(bumpNormal);

#ifdef _USE_SHADOWS
	input.shadowCoord.xyz /= input.shadowCoord.w;
	//transform clip space coords to texture space coords (-1:1 to 0:1)
	input.shadowCoord.x = input.shadowCoord.x/2 + 0.5f;
	input.shadowCoord.y = input.shadowCoord.y/-2 + 0.5f;
	
	input.shadowCoord.z -=.001f;
	//sample shadow map - point sampler
	//float shadowMapDepth = shadowMap.Sample(SampleType, input.shadowCoord.xy).r;// +.001f;

	float sum = 0;
	float x, y;
	for (y = -1.5; y <= 1.5; y += 1.0)
    {
        for (x = -1.5; x <= 1.5; x += 1.0)
        {
         //   sum += shadowMap.SampleCmpLevelZero( cmpSampler, input.lpos.xy + texOffset(x,y), input.lpos.z );
			sum += shadowMap.SampleCmpLevelZero( cmpSampler, input.shadowCoord.xy + texOffset(x,y), input.shadowCoord.z );
        }
    }

    float shadowMapDepth = sum / 16.f;	

	if(input.shadowCoord.x < -1.0f || input.shadowCoord.x > 1.0f ||
		input.shadowCoord.y < -1.0f || input.shadowCoord.y > 1.0f ||
		input.shadowCoord.z < 0.0f  || input.shadowCoord.z > 1.0f)
	{
		shadowMapDepth = 10000000;
	}

	if(shadowMapDepth >= input.shadowCoord.z && sunDirection.y > 0)
	{
		const float cosFactor = dot(input.normal, sunDirection.xyz);
		if(cosFactor > 0)
		{
			const float4 diffuseContribution = baseColor * sunColor * cosFactor;
			outputColor+=  diffuseContribution;
		}
	}
#else
	float cosFactor = dot(input.normal, sunDirection.xyz);
	//cosFactor = .65f;
		if(cosFactor > 0)
		{
			const float4 diffuseContribution = baseColor * sunColor * cosFactor;
			outputColor+=  diffuseContribution;
		}
#endif
    {
		
		const float len = distance(input.worldPosition, light0Position.xyz);
        //if(len < 1000)
        {
			float3 lightDirection = light0Position.xyz - input.worldPosition;
			lightDirection = normalize(lightDirection);
			const float cosFactor = dot(input.normal, lightDirection);

			if(cosFactor > 0)
			{
				const float lenFactor = min(1 / (.0001f * len*len), 1);
				const float4 diffuseContribution = baseColor * sunColor/*light0Color*/ * cosFactor * lenFactor;
				outputColor+=  diffuseContribution;
				const float3 r = normalize(reflect(lightDirection, input.normal));
				const float s = pow(max(dot(r, normalize(input.worldPosition - eyePoint.xyz)),0.0), 55);
				outputColor+= sunColor/*float4(.7f, .7f, .7f, 0)*/ * s * lenFactor;
			}
		}
    }
	
	/*const float cameraDistance = distance(eyePoint.xyz, input.worldPosition);
	if(cameraDistance < 1300)
	{
        const float3 lightDirection = normalize(eyePoint.xyz - input.worldPosition);
		const float cosFactor = dot(input.normal, lightDirection);

		if(cosFactor > 0)
		{
			const float lenFactor = min(1 / (.0001f * cameraDistance*cameraDistance), 1);
			const float4 diffuseContribution = (baseColor) * float4(1, 1, 1, 0) * cosFactor * lenFactor;
			outputColor+=  diffuseContribution;

			const float3 r = normalize(reflect(lightDirection, input.normal));
			const float s = pow(max(dot(r, normalize(input.worldPosition.xyz - eyePoint.xyz)),0.0), 55);
			outputColor+= float4(.7f, .7f, .7f, 0) * s * lenFactor;
		}
		
    }*/
	return clamp(outputColor, 0, 1);
}
 