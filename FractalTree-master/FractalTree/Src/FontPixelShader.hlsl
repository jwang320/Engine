Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct UIVertexShaderOutput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texCoord : TEXOORD0;
};

float4 main(UIVertexShaderOutput input) : SV_TARGET
{
	float4 outputColor = input.color;
	
	float4 color = float4(0,0,0,0);
	
	
	// Sample the texture pixel at this location.
	color = shaderTexture.Sample(SampleType, input.texCoord);
	if(color[0] == 0 && color[1] == 0 && color[2] == 0)
	{
		color = input.color;
		color.w = 0;
	}
	color.r = .2; color.g = .8; color.b = .2;
	return color;
}