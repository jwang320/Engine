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
normal.w = 0;
normal = normalize(normal);

{
	float4 lightDirection = light0Position - input.worldPosition;
	const float len = distance(input.worldPosition.xyz, light0Position.xyz);
	//if (len < 50000)
	{
		lightDirection.w = 0;
		lightDirection = normalize(lightDirection);
		float cosFactor = dot(normal, lightDirection);
		if (cosFactor >= 0)
		{
			const float lenPower = pow(len, 1.2f);//1.4f
			const float distCoefficient = .0001f;
			const float lenFactor = min(1 / (distCoefficient * lenPower), 1);
			float4 diffuseContribution = input.color * light0Color * cosFactor *lenFactor;
				outputColor += diffuseContribution;
			float4 r = normalize(reflect(lightDirection, input.normal));
				float num = dot(r.xyz, normalize(input.worldPosition.xyz - eyePoint.xyz));

				/*if (num > 0)
				{
					const float s = pow(abs(num), input.specularExponent);
					outputColor += input.specularColor * s * lenFactor;
				}*/
			}
		}

		{
		/*const float cosFactor = dot(input.normal, sunDirection.xyz);
		if(cosFactor > 0)
		{
			const float4 diffuseContribution = input.color * sunColor * cosFactor;
			outputColor+=  diffuseContribution;
		}*/
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
