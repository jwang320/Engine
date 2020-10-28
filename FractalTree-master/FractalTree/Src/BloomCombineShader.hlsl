
Texture2D screenQuadTexture : register(t11);
Texture2D bloomTexture : register(t12);
Texture2D blurTexture : register(t13);
Texture2D luminanceTexture : register(t14);
SamplerState SampleType : register(s0);
SamplerState PointSampler : register(s3);
SamplerState g_samPoint
{
    Filter = MIN_MAG_MIP_LINEAR; 
   /*AddressU  = Clamp;
   AddressV  = Clamp;*/
    AddressU = Wrap;
    AddressV = Wrap;
};
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

float4 AdjustSaturation(float4 color, float saturation)

{
    // We define gray as the same color we used in the grayscale shader
    float grey = dot(color, float4(0.3, 0.59, 0.11, 0));
   
    return lerp(grey, color, saturation);
}


static const float  MIDDLE_GRAY = 0.72f;
static const float  LUM_WHITE = 1.5f;
static const float4 LUM_VECTOR = float4(.299, .587, .114, 0);
float4 main(PixelShaderInput input) : SV_TARGET
{
	const float4 baseColor =  screenQuadTexture.Sample(SampleType, input.texCoord);
	float4 bloomColor = blurTexture.Sample(SampleType, input.texCoord);

	float4 luminance = luminanceTexture.SampleLevel(g_samPoint, input.texCoord, 0);
	luminance.a = 1;
	//return luminance;
	//return luminance;
	float LScale = dot(baseColor, LUM_VECTOR);
	LScale *= MIDDLE_GRAY / luminance.r;
	LScale = (LScale + LScale * LScale / LUM_WHITE * LUM_WHITE) / (1.0f + LScale);

	float4 toneMappedColor = baseColor * LScale;//baseColor *MIDDLE_GRAY / (luminance.r + 0.001f);
    /*toneMappedColor.rgb *= (1.0f + toneMappedColor/LUM_WHITE);
    toneMappedColor.rgb /= (1.0f + toneMappedColor);*/
    
    toneMappedColor.a = 1.0f;    

		// Controls the Intensity of the bloom texture
	const float BloomIntensity = 1.0;
 
	// Controls the Intensity of the original scene texture
	const float OriginalIntensity = 1.0;
 
	// Saturation amount on bloom
	const float BloomSaturation = 1.0;
 
	// Saturation amount on original scene
	const float OriginalSaturation = 1.0;



	bloomColor = AdjustSaturation(bloomColor, BloomSaturation) * BloomIntensity;
	toneMappedColor = AdjustSaturation(toneMappedColor, OriginalSaturation) * OriginalIntensity;
	toneMappedColor *= (1 - saturate(bloomColor));

	float4 combinedColor = toneMappedColor + bloomColor;
	combinedColor.a = 1;
	return combinedColor;




	const float averageLuminance = exp(luminance.x);
	const float key = max(0, 1.5f - (1.5f / (averageLuminance * .1f + 1))) + .1f;


	const float lScaled = key * dot(baseColor.xyz, float3(.27f, .67f, .06f)) / averageLuminance;
	

	//float4 averageBrightness = screenQuadTexture.SampleLevel(g_samPoint, input.texCoord, 11);
	//return averageBrightness;
	//const float mag = length(abs(averageBrightness.xyz));
	//return baseColor * float4(averageLuminance, averageLuminance, averageLuminance, 1);
	//return baseColor/float4(mag, mag, mag, 1);
	//return baseColor;
	
	//return bloomColor;
	/*bloomColor = AdjustSaturation(bloomColor, BloomSaturation) * BloomIntensity;
	baseColor = AdjustSaturation(baseColor, OriginalSaturation) * OriginalIntensity;
	//baseColor *= (1 - saturate(bloomColor));
	//screenQuadTexture.SampleLevel(g_samPoint, input.texCoord, 11);//
	
	float4 combinedColor = baseColor;// + bloomColor;
	combinedColor.a = 1;
	//return combinedColor;

	float exposure = lScaled;
//	if(mag > .5f)
//		exposure = 2;
	float2 vtc = float2( input.texCoord - 0.5 );
	float vignette = pow( 1 - ( dot( vtc, vtc ) * 1.0 ), 2.0 );
	float4 exposed = 1.0 - pow( 2.71, -(/ *vignette * * / combinedColor * exposure ) );

	return exposed;
	
	return combinedColor;*/
}