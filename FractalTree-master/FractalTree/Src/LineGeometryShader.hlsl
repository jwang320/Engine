//#include "SimpleVertexShader.hlsl"

struct GSOutput
{
	float4 pos : SV_POSITION;
	float4 blend : COLOR0;

	float4 worldPos : WORLD_POSITION;
};

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix view;
    matrix projection;
	float4 eyePoint;

	float4 light0Position;
	float4 light0Color;

	float pulseY;
};

struct GSInput
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD_POSITION;
};

[maxvertexcount(6)]
void main( line GSInput input[2], inout TriangleStream<GSOutput> triStream )
{
	float3 midpoint = (input[0].pos.xyz + input[1].pos.xyz)/2.0f;
	// since the vertices are all in view space
	// the eye is at 0,0,0
	float3 toEye = -midpoint.xyz; // midpoint to eye vector
	float3 delta = input[1].pos.xyz - input[0].pos.xyz;

	// p is the normalized perpendicular
	float3 p = normalize(cross(delta,toEye));
	//if(abs(input[0].worldPos.y - pulseY) < 5)
	/*if(pulseY % 20 > 10)
	{
		p = p * (((pulseY / 2)  - 15)% 10);
	}
	else*/
	{ 
		p = p * ((pulseY / 15) % 2);
	}
	GSOutput a;
	a.pos.w = 1.0f;

	
	float3 t;
	
	a.worldPos = input[0].worldPos;
	a.blend = float4(1,1,1,0);

	t = input[0].pos.xyz + p;
	a.pos = mul(float4(t.x,t.y,t.z,1.0f), projection);
	a.pos /= a.pos.w;
	triStream.Append(a);

	a.worldPos = input[1].worldPos;
	t = input[1].pos.xyz + p;
	a.pos = mul(float4(t.x,t.y,t.z,1.0f), projection);
	a.pos /= a.pos.w;
	triStream.Append(a);

	a.worldPos = input[0].worldPos;
	a.blend.w = 1;
	t = input[0].pos.xyz;
	a.pos = mul(float4(t.x,t.y,t.z,1.0f), projection);
	a.pos /= a.pos.w;
	triStream.Append(a);

	a.worldPos = input[1].worldPos;
	t = input[1].pos.xyz;
	a.pos = mul(float4(t.x,t.y,t.z,1.0f), projection);
	a.pos /= a.pos.w;
	triStream.Append(a);

	a.worldPos = input[0].worldPos;
	a.blend.w = 0;
	t = input[0].pos.xyz - p;
	a.pos = mul(float4(t.x,t.y,t.z,1.0f), projection);
	a.pos /= a.pos.w;
	triStream.Append(a);

	a.worldPos = input[1].worldPos;
	t = input[1].pos.xyz - p;
	a.pos = mul(float4(t.x,t.y,t.z,1.0f), projection);
	a.pos /= a.pos.w;
	triStream.Append(a);


	//triStream.RestartStrip();
}