Texture2D shaderTexture : register(t1);
Texture2D bumpTexture : register(t6);
Texture2D explosionTexture : register(t8);
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

float noise(int x, int z)
{
	int n = x + z * 57;
	n = (n << 13) ^ n;
	return (1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float4 main(PixelShaderInput input) : SV_TARGET
{
	//float2 newCoord = float2(noise(input.worldPosition.x, input.worldPosition.y), noise(input.worldPosition.x, input.worldPosition.y));
	//float h = abs(noise((input.texCoord.x - .5f)*(d2%100), (input.texCoord.y-.5f)*(d2%100)))*.01f;
	float ang = d2*3.14159265359f/180.f;
	//float h = noise((d2%1000)*.01f, (d2%1000)*.01f);
	//input.color = shaderTexture.Sample(SampleType, input.texCoord)*input.color;

	//float h = noise(input.texCoord.x*10, input.texCoord.y*10);
	float centerLen = length(float2(.5f, .5f)  - input.texCoord);
	float h = noise(centerLen*100, centerLen*100)*centerLen;
	input.color.a -= centerLen*2;

	//input.color.a = 1;
	//input.color.a = clamp(input.color.a, 0, 1);
	//input.color.a -= .525f;
	//input.color.a = min(input.color.a, .01f);
//	input.color
	//return float4(1, 1, 1, 1);
	//if(input.color.a < .25f)
	{
		//input.color.a = 0;
		//return input.color;
	}
	//input.color.a = .7f;
	//float4 outputColor = float4((input.color + float4(.25f, .25f, .25f, 0)).xyz*3, input.color.a);
	float4 outputColor = float4((input.color*2).xyz, input.color.a);
	//float4 outputColor = explosionTexture.Sample(SampleType, input.texCoord) * input.color;
   /* {
		float4 normal = normalize(float4(input.normal[0], input.normal[1], input.normal[2], 0));
		float4 bumpMap = bumpTexture.Sample(SampleType, input.texCoord*2);
		bumpMap = (bumpMap * 2.0f) - 1.0f;
		float4 bumpNormal = normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
		normal = normalize(bumpNormal);
// 		newNormal = input.normal + (input.tangent * newNormal.x + newNormal.z * input.binormal);
// 		normal = normalize(newNormal);

		normal.w = 0;

        float4 lightDirection = light0Position - input.worldPosition;
		lightDirection.w = 0;
		lightDirection = normalize(lightDirection);
        float cosFactor = dot(normal, lightDirection);

        if(cosFactor >= 0)
        {
			float4 diffuseContribution = input.color * light0Color * cosFactor; //input.color * float4(light0Color.r * cosFactor, light0Color.g * cosFactor, light0Color.b * cosFactor, 1);
            outputColor+=  diffuseContribution;
            float4 r = normalize(reflect(lightDirection, normal));
            float s = pow(max(dot(r, normalize(input.worldPosition - eyePoint)),0.0), input.specularExponent);
            outputColor+= input.specularColor * s;
        }
    }*/

	//outputColor+= input.emissiveColor*.1f;
	//outputColor.a = input.color.a;
	return clamp(outputColor, 0, 1);
}
 