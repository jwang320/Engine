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

cbuffer OncePerModelBuffer : register(b1)
{
	matrix model;
	matrix scaleMatrix;
	float4 tint;
};

cbuffer OncePerResizeBuffer : register(b2)
{
    matrix projection;
};

struct VertexShaderInput
{
    float4 pos : POSITION;
	float2 texCoord : TEXCOORD0;
};

struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
	float2 texCoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float4 pos = input.pos;
	//pos.w = 0;
	pos.w = 1;
	//pos[3] = 1;
    // Transform the vertex position into projected space.
    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);
    output.pos = pos;

    // Pass through the color without modification.
    output.color = float4(0, 1, 0, 0);
	output.texCoord = input.texCoord;
	//output.color.a = .25f;
    return output;
}
