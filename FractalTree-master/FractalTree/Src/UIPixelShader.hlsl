//SamplerState SampleType : register(s1);
Texture2D shaderTexture4 : register(t1);
SamplerState SampleType
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct UIVertexShaderOutput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texCoord : TEXOORD0;
};

float4 main(UIVertexShaderOutput input) : SV_TARGET
{
	
	float4 outputColor = input.color;
	if (outputColor.r < .2f && outputColor.g < .2 && outputColor.b < .2)
	{
		outputColor = float4(.1f, .2f, 1.f, .75f);
	}
	if(outputColor.a < .5f)
	{
		outputColor = shaderTexture4.Sample(SampleType, input.texCoord);
	}

	return clamp(outputColor, .5, 1);
}