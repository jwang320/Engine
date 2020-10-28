cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix view;
    matrix projection;
	float4 eyePoint;

	float4 light0Position;
	float4 light0Color;

	//float pulseY;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float4 blend : COLOR0;
	float4 worldPos : WORLD_POSITION;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
	float4 normal : NORMAL;

	
};


float gauss(float x)
{
	// normal gaussain

	float a = 1.0f / sqrt(2.0f*3.14159265f);
	float b = 1.0f;//x*x/-2.0f;
	return a*b;
}

float4 main(GSOutput input) : SV_TARGET
{
	//return float4(input.blend.w,input.blend.w,input.blend.w,input.blend.w);

//	col *= input.world;

	/*float diff = pulseY - input.worldPos.y;


	float g0 = 1.0f / sqrt(2.0f*3.14159265f);
	float g = g0*exp(diff*diff/-2.0f);
	float u = 1.0f - g0; //1.0f-gauss(0.0f);
//	float4 col = float4(1,1,1,input.blend.w);
	float4 col = float4(u + g, u + g, u + g,input.blend.w);
	//col = float4(1,1,1,1);
	return col;*/
	return float4(1,1,1,1);
	//return float4(
	//			1.0f,
	//	input.blend.w,
	//	input.blend.w * (1.0f) + 0.5f * (1- input.blend.w),

	//	input.blend.w);//*;
}