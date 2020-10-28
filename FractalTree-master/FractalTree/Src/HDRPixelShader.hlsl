
Texture2D screenQuadTexture : register(t11);
Texture2D luminanceTexture : register(t14);
SamplerState SampleType : register(s0);
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

SamplerState g_samPoint
{
    Filter = MIN_MAG_POINT_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
static const float4 LUM_VECTOR = float4(.299, .587, .114, 0);

float4 main(PixelShaderInput input) : SV_TARGET
{
	const float blurDist = .002f;
	const float Threshold = .6f;
	const float bloomFactor = 1.0f;
	float4 baseColor =  screenQuadTexture.Sample(SampleType, input.texCoord);
	/*float lum = dot(baseColor.rgb, LUM_VECTOR);
	float avgLum = luminanceTexture.SampleLevel(g_samPoint, input.texCoord, 11).r;

	float colorScale = saturate(lum - avgLum * Threshold);

	float4 finalColor = baseColor * colorScale;
	finalColor.a = 1;

	return finalColor;*/

	baseColor.a = 1;
	return saturate((baseColor - Threshold) / (1 - Threshold));	
}