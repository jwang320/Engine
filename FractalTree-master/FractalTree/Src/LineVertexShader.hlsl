cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix view;
    matrix projection;
	float4 eyePoint;

	float4 light0Position;
	float4 light0Color;
};

cbuffer OncePerModelBuffer : register(b1)
{
	matrix model;
};
struct VertexShaderInput
{
    float4 pos : POSITION;
    float4 color : COLOR0;
	float4 normal : NORMAL;
};

struct GSInput
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD_POSITION;
};

GSInput main(VertexShaderInput input)// : SV_POSITION
{
	GSInput output;

    float4 pos = input.pos;

    // Transform the vertex position into projected space.
    pos = mul(pos, model);
	output.worldPos = pos;
    pos = mul(pos, view);

	output.pos = pos;


	return output;
}
