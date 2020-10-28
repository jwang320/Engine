
Texture2D screenQuadTexture : register(t11);

SamplerState SampleType : register(s0);
SamplerState PointSampler : register(s3);
SamplerState g_samPoint
{
    Filter = MIN_MAG_POINT_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

static const float4 LUM_VECTOR = float4(.299, .587, .114, 0);

float4 main(PixelShaderInput input) : SV_TARGET
{
	const float delta = .0001f;
	//const float L = log(delta + dot(float3(.27f, .67f, .06f), screenQuadTexture.Sample(SampleType, input.texCoord).xyz));
	const float L = dot(LUM_VECTOR, screenQuadTexture.Sample(SampleType, input.texCoord));
	return float4(L, L, L, 1);
}