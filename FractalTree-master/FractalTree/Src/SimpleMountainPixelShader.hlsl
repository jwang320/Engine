Texture2D shaderTexture : register(t1);
Texture2D shaderTexture2 : register(t2);
Texture2D alphaMap : register(t3);
Texture2D shaderTexture3 : register(t4);
Texture2D shaderTexture4 : register(t5);
Texture2D shaderTexture5 : register(t7);
Texture2D bumpTexture : register(t6);
Texture2D shadowMap : register(t10);
//Texture2D bumpTexture : register(t2);
SamplerState SampleType : register(s1);

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

static const int grad3[12][3] = {{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
	{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
	{0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1}};

cbuffer PerlinBuffer : register(b3)
{
	float4 textureRepeatCount;
};

/*
cbuffer SimplexBuffer : register(b4)
{
	int perm[512];
};*/

static const uint perm[512] = {151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 
	103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 
	75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 
	149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 
	27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 
	55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 
	187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 
	173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 
	85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 
	119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 
	39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 
	228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 
	249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 
	121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 
	128, 195, 78, 66, 215, 61, 156, 180, 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 
	194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 
	120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 
	56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 
	231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 
	25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 
	86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 
	82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 
	44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 
	232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 
	51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 
	127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180};

struct TexturedPixelShaderInput
{
    float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 worldPosition : WORLD_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 shadowCoord : TEXCOORD1;
};
SamplerComparisonState cmpSampler : register(s2)
{
   // sampler state
   Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
   AddressU = MIRROR;
   AddressV = MIRROR;
 
   // sampler comparison state
   ComparisonFunc = LESS_EQUAL;
};
float2 texOffset( int u, int v )
{
    return float2( u * 1.0f/1024, v * 1.0f/1024);
}

#define _USE_NORMAL_MAPs
#define _USE_SPECULAR
#define _USE_LIGHTING
#define _USE_SHADOWSs
#define _USE_TEXTURES
#define _USE_FOG

int fastfloor(float x) 
{
	return x>0 ? int(x) : int(x)-1;
}

float calcSimplex(float newX, float newZ)
{
	float n0, n1, n2;
	static const float F2 = 0.5f*(sqrt(3.0f)-1.0f);

	const float s = (newX + newZ)*F2;
	const int i = fastfloor(newX + s);
	const int j = fastfloor(newZ + s);

	static const float G2 = (3.f - sqrt(3.f)) / 6.f;
	const float t = (i + j) * G2;
	const float X0 = i-t; // Unskew the cell origin back to (x,z) space
	const float Z0 = j-t;
	const float x0 = newX-X0; // The x,z distances from the cell origin
	const float z0 = newZ-Z0;
 
	//const XMVECTOR X0 = XMVectorSet(i - t, 0, j - t, 0);
	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if(x0>z0) {i1=1; j1=0;} // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else {i1=0; j1=1;} // upper triangle, YX order: (0,0)->(0,1)->(1,1)

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6
	const float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	const float z1 = z0 - j1 + G2;
	const float x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
	const float z2 = z0 - 1.0f + 2.0f * G2;

	// Work out the hashed gradient indices of the three simplex corners
	int ii = i & 255;
	int jj = j & 255;
	int gi0 = perm[ii+perm[jj]] % 12;
	int gi1 = perm[ii+i1+perm[jj+j1]] % 12;
	int gi2 = perm[ii+1+perm[jj+1]] % 12;

	// Calculate the contribution from the three corners
	float t0 = 0.5f - x0*x0-z0*z0;
	if(t0<0) n0 = 0.0f;
	else {
		t0 *= t0;
		//n0 = t0 * t0 * dot(grad3[gi0], x0, y0); // (x,y) of grad3 used for 2D gradient
		n0 = t0 * t0 * (grad3[gi0][0] * x0 + grad3[gi0][1] * z0);
	}

	float t1 = 0.5f - x1*x1-z1*z1;
	if(t1<0) n1 = 0.0f;
	else {
		t1 *= t1;
		n1 = t1 * t1 * (grad3[gi1][0] * x1 + grad3[gi1][1] * z1);//dot(grad3[gi1], x1, y1);
	}

	float t2 = 0.5f - x2*x2-z2*z2;
	if(t2<0) n2 = 0.0f;
	else {
		t2 *= t2;
		n2 = t2 * t2 * (grad3[gi2][0] * x2 + grad3[gi2][1] * z2);//dot(grad3[gi2], x2, z2);
	}

	return  70.0f * (n0 + n1 + n2);
}

float GetHeight(float x, float z, float depth)
{
	const int octaveCount = 7;
	const float persistance = .5;
	const float frequencyMultiplier = 1;
	const float scale = 1;

	float h = 0;
	const float amplitude[octaveCount] = {.25, .15, .075, .0325, .01125, .005, .0025};
	const float frequency[octaveCount] = {.1, .2, .4, .8, 1.6, 3.2, 6.4};
	const float maxDepth[octaveCount] = {500, 500, 500, 150, 100, 70, 40};
	for(int i = 0; i < octaveCount; ++i)
	{
		/*const float amplitude = pow(persistance, i) * scale;
		const float frequency = pow(2, i) / scale * frequencyMultiplier;*/

		if(depth < maxDepth[i])
		{
			h += calcSimplex(x * frequency[i], z * frequency[i]) * amplitude[i];
		}
	}

	return h;
}

float3 GetNormal(float xLoc, float zLoc, float dist, float depth)
{
	const float h0 = GetHeight(xLoc, zLoc - dist, depth);
	const float h1 = GetHeight(xLoc - dist, zLoc, depth);
	const float h2 = GetHeight(xLoc + dist, zLoc, depth);
	const float h3 = GetHeight(xLoc, zLoc + dist, depth);

	const float3 normal = float3(h1 - h2, 2 * dist, h0 - h3);
	return normalize(normal);
}

float4 main(TexturedPixelShaderInput input) : SV_TARGET
{
	const float4 fogColor = float4(clamp(sunDirection.y, 0, 1), clamp(sunDirection.y, 0, 1), clamp(sunDirection.y, 0, 1), 1);
	const float fogDist = 100000;
	const float cameraDistance = distance(eyePoint.xyz, input.worldPosition);
	//if(cameraDistance > fogDist)
	{
	//	return fogColor;
	}

#ifdef _USE_FOG
	const float fogDensity = .00001f * textureRepeatCount.y;//.00012f;//.00006f;
	//float fogFL = (fogDist - cameraDistance) / fogDist;
	float fogFE = pow(1.0 / 2.71828, cameraDistance * fogDensity * cameraDistance * fogDensity);//cameraDistance * fogDensity);
	if(fogFE < .015f)
	{
		return fogColor;
	}
#endif

	

	const float terrainMag = .75f;
	const float4 alp =  alphaMap.Sample(SampleType, input.texCoord / textureRepeatCount.x);
	float4 baseColor = float4(terrainMag, terrainMag, terrainMag, 1);

	//float4 outputColor = baseColor*max(0, sunDirection.y)*sunColor*.15;//float4(0, 0, 0, 1);
	float4 outputColor = float4(0, 0, 0, 1);

	input.normal = normalize(input.normal);
	//input.shadowCoord.z -=.001f;
	//sample shadow map - point sampler

#ifdef _USE_NORMAL_MAP
	//if(cameraDistance < 1000)
	{
		float3 bumpMap = bumpTexture.Sample(SampleType, input.texCoord).xyz;
		bumpMap = (bumpMap * 2.0f) - 1.0f;
		input.normal = normalize(input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal);
	}

#else
		/*float he = GetHeight(input.worldPosition.x , input.worldPosition.z);
		he = (he + 1) / 2;
		return float4(he, he, he, 1);*/
		//float3 bumpMap = GetNormal(input.worldPosition.x, input.worldPosition.z, .01, cameraDistance);
		
		//bumpMap = (bumpMap * 2.0f) - 1.0f;
		//input.normal = float4(bumpMap, 0);
		//return float4(-input.binormal, 1);
		//bumpMap = float3(0, 1, 0);
		//input.normal = normalize(input.normal + bumpMap.x * input.tangent + bumpMap.z * input.binormal);//normalize(input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal);
		//
		//input.normal = bumpMap;
		//return float4(abs(input.normal), 1);
#endif
	/*const float4 greenColor = float4(.25, 1, .25, 1);
	const float4 brownColor = float4(155 / 255.f, 118 / 255.f, 83 / 255.f, 1);
	const float4 blackColor = float4(49/255.f, 79/255.f, 79/255.f, 1);*/
	
	/*const float4 greenColor = float4(0, .5, 0, 1); // shaderTexture2.Sample(SampleType, input.texCoord);
	const float4 brownColor = float4(normalize(float3(225, 169, 95)), 1);// shaderTexture.Sample(SampleType, input.texCoord);
	const float4 solidBlackColor = float4(.39f, .41f, .44f, 1);
	const float4 blackColor = float4(.39f, .41f, .44f, 1); //shaderTexture4.Sample(SampleType, input.texCoord);// solidBlackColor;// input.normal.y < .5f ? solidBlackColor : (1 - 10 * (.6f - input.normal.y))*shaderTexture4.Sample(SampleType, input.texCoord) + 10 * (.6f - input.normal.y)*solidBlackColor;
	*/
	const float4 greenColor =  shaderTexture2.Sample(SampleType, input.texCoord);
	const float4 brownColor =  shaderTexture.Sample(SampleType, input.texCoord);
	const float4 solidBlackColor = float4(.39f, .41f, .44f, 1);
	const float4 blackColor = shaderTexture4.Sample(SampleType, input.texCoord);// solidBlackColor;// input.normal.y < .5f ? solidBlackColor : (1 - 10 * (.6f - input.normal.y))*shaderTexture4.Sample(SampleType, input.texCoord) + 10 * (.6f - input.normal.y)*solidBlackColor;


#ifdef _USE_TEXTURES

	/*if(input.normal.y > .92f)
	{
		return greenColor;
		baseColor *= greenColor;
	}*/

	float specChange = 1;
	if (input.normal.y > .8f)
	{
		baseColor *= brownColor*((1 - input.normal.y) / .2f) + greenColor * ((input.normal.y - .8f) / .2f);
		specChange = 0;
	}
	else if (input.normal.y > .6f)
	{
		baseColor *= blackColor*((.8f - input.normal.y) / .2f) + brownColor * ((input.normal.y - .6f) / .2f);
		specChange = (.8f - input.normal.y) / .2f;
	}
	else if (input.normal.y < .6f)
	{
		baseColor *= blackColor;
	}

	baseColor.a = 1;
#endif

#ifdef _USE_LIGHTING
#ifdef _USE_SHADOWS
	//outputColor += baseColor*.2f;

	/*input.shadowCoord.xyz /= input.shadowCoord.w;
	
	input.shadowCoord.x = input.shadowCoord.x/2 + 0.5;
	input.shadowCoord.y = input.shadowCoord.y/-2 + 0.5;
	
	//float shadowMapDepth = shadowMap.Sample(SampleType, input.shadowCoord.xy).r +.001f;
	float shadowMapDepth = 0;
	//input.shadowCoord
	/*float sum = 0;
	float x, y;
	for (y = -1.5; y <= 1.5; y += 1.0)
    {
        for (x = -1.5; x <= 1.5; x += 1.0)
        {
         //   sum += shadowMap.SampleCmpLevelZero( cmpSampler, input.lpos.xy + texOffset(x,y), input.lpos.z );
			sum += shadowMap.SampleCmpLevelZero( cmpSampler, input.shadowCoord.xy + texOffset(x,y), input.shadowCoord.z );
        }
    }

    float shadowMapDepth = sum / 16.f;*/

	/*if(input.shadowCoord.x < -1.0f || input.shadowCoord.x > 1.0f ||
			input.shadowCoord.y < -1.0f || input.shadowCoord.y > 1.0f ||
			input.shadowCoord.z < 0.0f  || input.shadowCoord.z > 1.0f)
	{
		shadowMapDepth = 10000000;
	}
	else
	{
		shadowMapDepth = shadowMap.Sample(SampleType, input.shadowCoord.xy).r;// +.001f;
		//shadowMapDepth = shadowMap.SampleCmpLevelZero(cmpSampler, input.shadowCoord.xy, input.shadowCoord.z).r;
	}*/
	float2 shadowTexCoords;
	shadowTexCoords.x = 0.5f + (input.shadowCoord.x / input.shadowCoord.w * 0.5f);
	shadowTexCoords.y = 0.5f - (input.shadowCoord.y / input.shadowCoord.w * 0.5f);
	float pixelDepth = input.shadowCoord.z / input.shadowCoord.w;
	float lighting = 1;

	// Check if the pixel texture coordinate is in the view frustum of the 
	// light before doing any shadow work.
	if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) &&
		(saturate(shadowTexCoords.y) == shadowTexCoords.y) &&
		(pixelDepth > 0))
	{
		// Use an offset value to mitigate shadow artifacts due to imprecise 
		// floating-point values (shadow acne).
		//
		// This is an approximation of epsilon * tan(acos(saturate(NdotL))):
		float margin = 1;// acos(saturate(NdotL));
#ifdef LINEAR
		// The offset can be slightly smaller with smoother shadow edges.
		float epsilon = 0.0005 / margin;
#else
		float epsilon = 0.001 / margin;
#endif
		// Clamp epsilon to a fixed range so it doesn't go overboard.
		epsilon = clamp(epsilon, 0, 0.1);
		epsilon = .001f;
		// Use the SampleCmpLevelZero Texture2D method (or SampleCmp) to sample from 
		// the shadow map, just as you would with Direct3D feature level 10_0 and
		// higher.  Feature level 9_1 only supports LessOrEqual, which returns 0 if
		// the pixel is in the shadow.
		/*lighting = shadowMap.SampleCmpLevelZero(
			cmpSampler,
			shadowTexCoords,
			pixelDepth + epsilon
			).r;*/
		lighting = shadowMap.Sample(SampleType, shadowTexCoords).r +.0001f;
			
	}
	if(lighting >= pixelDepth && sunDirection.y > 0)
	{
		const float cosFactor = dot(input.normal, sunDirection.xyz);
		if(cosFactor > 0)
		{
			const float4 diffuseContribution = baseColor * sunColor * sqrt(cosFactor);
			outputColor+=  diffuseContribution;
#ifdef _USE_SPECULAR
			const float3 r = normalize(reflect(sunDirection.xyz, input.normal));
			const float num = dot(r, normalize(input.worldPosition - eyePoint.xyz));

			if(num > 0)
			{
				const float s = pow(num, 55);
				outputColor+= sunColor/*float4(.7f, .7f, .7f, 0)*/ * s * .25f;
			}
#endif
		}
	}
	else
	{
		outputColor += baseColor*.2f;
	}
#else
	if(sunDirection.y > 0)
	{
		const float cosFactor = dot(input.normal, sunDirection.xyz);
		if(cosFactor > 0)
		{
			const float4 diffuseContribution = baseColor * sunColor * cosFactor;
			outputColor+=  diffuseContribution;
#ifdef _USE_SPECULAR
			const float3 r = normalize(reflect(sunDirection.xyz, input.normal));
			const float num = dot(r, normalize(input.worldPosition - eyePoint.xyz));

			if(num > 0)
			{
				const float s = pow(num, 55);
				outputColor+= sunColor/*float4(.7f, .7f, .7f, 0)*/ * s * .25f * specChange;
			}
		}
#endif
	}
#endif
    {
		const float len = distance(input.worldPosition, light0Position.xyz);
        //if(len < 1000)
        {
			const float3 lightDirection = normalize(light0Position.xyz - input.worldPosition);
			const float cosFactor = dot(input.normal, lightDirection);

			if(cosFactor > 0)
			{				
				const float lenFactor = min(1 / (.0001f * len*len), 1);
				const float4 diffuseContribution = baseColor * light0Color * cosFactor * lenFactor;
				outputColor+=  diffuseContribution;
				
				const float3 r = normalize(reflect(lightDirection, input.normal));
				const float num = dot(r, normalize(input.worldPosition - eyePoint.xyz));

				if(num > 0)
				{
					const float s = pow(num, 55);
					outputColor+= float4(.7f, .7f, .7f, 0) * s * lenFactor;
				}
			}
        }
    }

	/*if(cameraDistance < 1300)
	{
        const float3 lightDirection = normalize(eyePoint - input.worldPosition);
		const float cosFactor = dot(input.normal, lightDirection);

		if(cosFactor > 0)
		{
			const float lenFactor = min(1 / (.0001f * cameraDistance*cameraDistance), 1);
			const float4 diffuseContribution = baseColor * cosFactor * lenFactor;
			outputColor+=  float4(diffuseContribution.xyz, 0);

			//const float4 r = normalize(reflect(lightDirection, input.normal));
			//const float s = pow(max(dot(r, normalize(input.worldPosition - eyePoint)),0.0), input.specularExponent);
			//outputColor+= float4(.7f, .7f, .7f, 0) * s * lenFactor;
		}
		
    }*/
#endif
#ifdef _USE_FOG
	
	const float4 newColor = fogFE * outputColor + (1 - fogFE) * fogColor;
	//outputColor+= 1.0 / 2.71828 * (cameraDistance * 2) * (cameraDistance * 2) * float4(1, 1, 1, 1);
	outputColor = newColor;
#endif
	
	outputColor.a = 1;
	return outputColor;//clamp(outputColor, 0, 1);
}