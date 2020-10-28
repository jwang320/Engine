struct VertexShaderInput
{
    float4 pos : POSITION;
	float2 texCoord : TEXCOORD0;
};


struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	output.pos = input.pos;
	output.texCoord = input.texCoord;
	return output;
}