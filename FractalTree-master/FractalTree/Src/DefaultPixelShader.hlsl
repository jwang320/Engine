Texture2D shaderTexture : register(t1);
Texture2D bumpTexture : register(t2);
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
	float4 light1Position;
	float4 light1Color;
	float4 customValueFrame;
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
	nointerpolation float4 customValue : COLOR4;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	if (input.emissiveColor.x + input.emissiveColor.y + input.emissiveColor.z > 0)
	{
		//input.emissiveColor.w = input.color.w;
		//return input.emissiveColor;
		//input.color.xyz *= .25f;
	}

	////return customValue;
	//return input.emissiveColor;
	const int customVal = input.customValue.x;
	int repeatIndex = 0;
	//float4 addColor = float4(0, 0, 0, 0);
	int texInd = input.texCoord.x * 100;
	//return float4(input.texCoord.y, 0, 0, 1);
	//return float4
	if (customVal > 0 && input.normal.x == 0)
	{
		/*repeatIndex = 100 / (customVal + 1);
		if (texInd.x > 6 && texInd.x < 94)
		{
			const int modVal = min(texInd % repeatIndex, abs(texInd % repeatIndex - repeatIndex));
			if (modVal <= 3)
				//if (texInd > 50)
			{
				//return float4(0, 0, 1, 1);
				//	input.color = float4(0, 0, 1.f / (modVal != 0 ? modVal : 1), 1);
				//input.emissiveColor = float4(0, 0, 1.f / (modVal != 0 ? modVal : 1), 1);
				//input.color -= input.emissiveColor;
				//input.color.a = 1;
				//input.color = float4(0, 0, 1, 1);
				//input.emissiveColor = float4(0, 0, .5f, 0);
				const float mag = 1;// modVal * .01f;// abs(500 - modVal) * 2 * .001f;//modVal != 0 ? 1.f / (1000 - modVal) : 1;
				return float4(0, 1 * (1 - abs(input.texCoord.y - input.customValue.z)), mag, 1);
			}
		}*/
		for (int a = 1; a < customVal + 1; ++a)
		{
			//if (abs(input.texCoord.x - a * .15) < .04)
			if (input.texCoord.x < a / 12.f)
			{
				float mag = 1;
				float d = (1 - abs(input.texCoord.y - input.customValue.z));
				//return float4(input.color.x * d, 1 * (1 - abs(input.texCoord.y - input.customValue.z)), (mag - d) + input.color.z*d, 1);
				return float4(0, 1 * (1 - abs(input.texCoord.y - input.customValue.z)), mag, 1);
			}
		}
	}
	else if (input.customValue.w > 0 && input.color.a < .92f)
	{
		int fCount = (((customValueFrame.x * 10) % 100));// % 100) * 10;
		/*int wPos = (input.worldPosition.x * 10) % fCount;
		float m = wPos / 100.f;
		return float4(m, 0, 0, 1);*/
		const int d = ((unsigned int) (input.worldPosition.x)) % 100;
		if (abs( d- fCount)*10 < 100)
		{
			//return float4(1 - abs(d - fCount) * 10 * .01f, 1 - abs(d - fCount) * 10 * .01f, 1 - abs(d - fCount) * 10 * .01f, .9f);
		}
	}

	float4 outputColor = input.ambientColor + input.emissiveColor;// +float4(input.texCoord.x, 0, 0, 0);// +input.customValue;
		float4 normal = float4(input.normal[0], input.normal[1], input.normal[2], input.normal[3]);
		normal.w = 0;
	normal = normalize(normal);
	//if (input.customValue.y < .5f)
	{
		float3 bumpMap = bumpTexture.Sample(SampleType, float2(input.texCoord.x / 10 * max(1, input.customValue.y) + .5f, input.texCoord.y / 10 * max(1, input.customValue.y))).xyz;
			bumpMap = (bumpMap * 2.0f) - 1.0f;
		normal = normalize(normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal);
	}
	//return float4(input.pos.x, 0, 0, 1);
	{
		float4 lightDirection = light0Position - input.worldPosition;
			const float len = distance(input.worldPosition.xyz, light0Position.xyz);
		if (len < 50000)
		{
			lightDirection.w = 0;
			lightDirection = normalize(lightDirection);
			float cosFactor = dot(normal, lightDirection);
			if (cosFactor >= 0)
			{
				const float lenFactor = min(1 / (.0001f * len*len), 1);
				float4 diffuseContribution = input.color * light0Color * cosFactor * lenFactor;
					outputColor += diffuseContribution;
				float4 r = normalize(reflect(lightDirection, normal));
					float num = dot(r.xyz, normalize(input.worldPosition.xyz - eyePoint.xyz));

				if (num > 0)
				{
					const float s = pow(abs(num), input.specularExponent);
					outputColor += input.specularColor * s;
				}
			}
		}

		{
			const float cosFactor = dot(normal, sunDirection.xyz);
			if(cosFactor > 0)
			{
			const float4 diffuseContribution = input.color * sunColor * cosFactor;
			outputColor+=  diffuseContribution;
			}
		}

	}
	{
		float4 lightDirection = light1Position - input.worldPosition;
			const float len = distance(input.worldPosition.xyz, light1Position.xyz);
		if (len < 50000)
		{
			lightDirection.w = 0;
			lightDirection = normalize(lightDirection);
			float cosFactor = dot(normal, lightDirection);
			if (cosFactor >= 0)
			{
				const float lenFactor = min(1 / (.0001f * len*len), 1);
				float4 diffuseContribution = input.color * light1Color * cosFactor * lenFactor;
					outputColor += diffuseContribution;
				float4 r = normalize(reflect(lightDirection, normal));
					float num = dot(r.xyz, normalize(input.worldPosition.xyz - eyePoint.xyz));

				if (num > 0)
				{
					const float s = pow(abs(num), input.specularExponent);
					outputColor += input.specularColor * s;
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

	outputColor.a = input.color.a;
	return clamp(outputColor, 0, 1);
}
