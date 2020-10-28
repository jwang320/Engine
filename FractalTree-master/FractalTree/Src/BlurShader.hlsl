
Texture2D screenQuadTexture : register(t11);
Texture2D bloomTexture : register(t12);
SamplerState SampleType : register(s0);
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	const float blurDist = .002f;
	const float Threshold = .8f;
	const float bloomFactor = 1.0f;
	float4 baseColor =  bloomTexture.Sample(SampleType, input.texCoord);
	baseColor.a = 1;
	float4 c1 = bloomTexture.Sample(SampleType, input.texCoord + float2(blurDist, 0));
	float4 c2 = bloomTexture.Sample(SampleType, input.texCoord + float2(-blurDist, 0));
	float4 c3 = bloomTexture.Sample(SampleType, input.texCoord + float2(0, blurDist));
	float4 c4 = bloomTexture.Sample(SampleType, input.texCoord + float2(0, -blurDist));
	
	float4 c5 = bloomTexture.Sample(SampleType, input.texCoord + float2(blurDist *2, 0));
	float4 c6 = bloomTexture.Sample(SampleType, input.texCoord + float2(-blurDist *2, 0));
	float4 c7 = bloomTexture.Sample(SampleType, input.texCoord + float2(0, blurDist *2));
	float4 c8 = bloomTexture.Sample(SampleType, input.texCoord + float2(0, -blurDist *2));
	
	float4 c9 = bloomTexture.Sample(SampleType, input.texCoord + float2(blurDist *3, 0));
	float4 c10 = bloomTexture.Sample(SampleType, input.texCoord + float2(-blurDist *3, 0));
	float4 c11 = bloomTexture.Sample(SampleType, input.texCoord + float2(0, blurDist *3));
	float4 c12 = bloomTexture.Sample(SampleType, input.texCoord + float2(0, -blurDist *3));

	float4 finalColor = (baseColor + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9 + c10 + c11 + c12) / 13;
	finalColor.a = 1;
	return finalColor;
}