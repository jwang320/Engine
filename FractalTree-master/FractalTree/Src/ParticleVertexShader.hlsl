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
	float4 diffuseTint;
	float4 customValue;
};

cbuffer OncePerResizeBuffer : register(b2)
{
    matrix projection;
};

struct ParticleVertexShaderInput
{
    float4 pos : POSITION;
    float4 color : COLOR0;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
    float4 emissiveColor : COLOR1;
	float4 specularColor : COLOR2;
	float4 ambientColor : COLOR3;
	float2 texCoord : TEXCOORD0;
	float specularExponent : NIL;
	float4 center : NILC;
};

struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
	float4 worldPosition : WORLD_POSITION;
    float4 emissiveColor : COLOR1;
	float4 specularColor : COLOR2;
	float4 ambientColor : COLOR3;
	float2 texCoord : TEXCOORD0;
	float specularExponent : NIL;
};

VertexShaderOutput main(ParticleVertexShaderInput input)
{
    VertexShaderOutput output;
	//pos.w = 0;
	//pos[3] = 1;
    // Transform the vertex position into projected space.

//     pos = mul(pos, model);
// 	output.worldPosition = pos;
//     
	
	//float4 newCenter = float4(250, 0, 250, 0);
	float4 newCenter = input.pos;
	//float3 lookVec = normalize(eyePoint.xyz - input.center.xyz);
	float3 lookVec = normalize(eyePoint.xyz - newCenter.xyz);
	float3 rightVec = normalize(cross(cameraUp.xyz, lookVec));
	float3 upVec = float3(0, 1, 0);//(cross(lookVec, rightVec));

	
	float4x4 rotMat = {rightVec.x, upVec.x, lookVec.x, input.center.x,
					   rightVec.y, upVec.y, lookVec.y, input.center.y,
					   rightVec.z, upVec.z, lookVec.z, input.center.z,
					   0		 , 0	  , 0		 , 1				};
	float4x4 rotMat2 = {rightVec.x, upVec.x, lookVec.x, 0,
	rightVec.y, upVec.y, lookVec.y, 0,
	rightVec.z, upVec.z, lookVec.z, 0,
	0		 , 0	  , 0		 , 1				};

	float d = cross(lookVec, input.normal.xyz).z > 0 ? -dot(lookVec, input.normal.xyz) : dot(lookVec, input.normal.xyz);
	//float c = cross(lookVec, input.normal.xyz).z;
	float yAng = acos(d);// : -acos(d);
	float4x4 rotMat3 = {d, 0, sin(yAng), 0,
						0, 1, 0, 0,
						-sin(yAng), 0, d, 0,
						0, 0, 0, 1};

	float4x4 rotMat4 = {rightVec.x, upVec.x, lookVec.x, input.pos.x,
					   rightVec.y, upVec.y, lookVec.y, input.pos.y,
					   rightVec.z, upVec.z, lookVec.z, input.pos.z,
					   0		 , 0	  , 0		 , 1				};
	float4x4 rotMat5 = {rightVec.x, upVec.x, lookVec.x, newCenter.x,
					   rightVec.y, upVec.y, lookVec.y, newCenter.y,
					   rightVec.z, upVec.z, lookVec.z, newCenter.z,
					   0		 , 0	  , 0		 , 1				};
	input.center.w = 1;
	float4 pos = input.pos;// - input.center + input.center;
	pos.w = 1;
  // pos = pos - newCenter;//input.center;
	pos = mul(pos, rotMat4);
	//pos = pos + newCenter;//input.center;
	//pos-= input.center;
	//pos+= input.center;
	pos.w = 1;
  // pos = mul(pos, transpose(rotMat2));
	//pos = pos+input.center;
	//pos.w = 1;
	output.worldPosition = pos;
	pos = mul(pos, view);
	pos = mul(pos, projection);
    output.pos = pos;

    // Pass through the color without modification.
	output.color = diffuseTint;// input.color;
	input.normal.w = 0;
	output.texCoord = input.texCoord;
	output.normal = normalize(mul(input.normal, model));
	output.tangent = normalize(mul(input.tangent, model));
	output.binormal = normalize(mul(input.binormal, model));
	output.emissiveColor = input.emissiveColor + tint;
	output.specularColor = input.specularColor;
	output.specularExponent = input.specularExponent;
	output.ambientColor = input.ambientColor;
	//output.color.a = .25f;
    return output;
}
