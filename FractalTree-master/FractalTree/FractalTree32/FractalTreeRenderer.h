#pragma once
#include "..\Common Source\Rendering Engine\Rendering Engine Sample\SampleRenderer.h"

namespace Geometry { class Mesh; class LSystemMesh; }

namespace RenderingEngine
{
	class ParticleSystem;
	struct TextureData;
	class UIRenderer;

	//FractalTreeRenderer contains application specific rendering
	//functionality, mainly relating to loading specific shaders, textures,
	//and handling special properties of objects.  For the functionality
	//of the renderer itself, see StandardCubeRenderer
	class FractalTreeRenderer : public SampleRenderer
	{
	public:
		enum PixelShaders
		{
			SimplePixelShader,
			SimpleMountainPixelShader,
			UIPixelShader,
			FontPixelShader,
			TreePixelShader,
			ParticlePixelShader,
			FoliagePixelShader,
			WireFramePixelShader,
			SkyPixelShader,
			BlurShader,
			BloomCombineShader,
			WaterPixelShader
		};

		enum VertexShaders
		{
			SimpleVertexShader,
			UIVertexShader,
			ParticleVertexShader,
			TerrainVertexShader,
			InstancedFoliageVertexShader,
			WorldFontVertexShader,
			WireFrameVertexShader,
			SkyVertexShader
		};

		enum InputLayouts
		{
			DefaultInputLayout,
			UIInputLayout,
			ParticleInputLayout,
			TerrainInputLayout,
			InstancedInputLayout,
			WorldFontInputLayout,
			WireFrameInputLayout,
			SkyInputLayout
		};

		enum Textures
		{
			FontTexture,
			MountainTexture,
			GrassTexture,
			SlabTexture,
			StoneTexture,
			BumpMapTexture,
			IceTexture,
			ExplosionTexture,
			LeafTexture1,
			LeafTextureNormal1,
			OrganTexture
		};

		enum SamplerStates
		{
			FontSamplerState,
			TerrainSamplerState
		};

		enum ResourceViews
		{
			FontResourceView,
			MountainResourceView,
			GrassResourceView,
			SlabResourceView,
			StoneResourceView,
			BumpmapResourceView,
			IceResourceView,
			ExplosionResourceView,
			LeafTexture1ResourceView,
			LeafTextureNormal1ResourceView,
			OrganTextureResourceView
		};

	protected:

		ID3D11Buffer* perlinBuffer = nullptr;

		PerlinBuffer perlinBufferData;
		float perlinFrequency;
		float perlinAmplitude = 9;
		int textureQualityIndex = 5;
		int terrainTextureRepeatIndex = 2;

	public:
		FractalTreeRenderer();
		
		virtual ~FractalTreeRenderer() override;

	protected:
		virtual void InitializeOther() override;
		virtual void InitializeShaders() override;
		virtual void InitializeTextures() override;

	private:
		bool initializeModels();

		//Shader initialization
		void initializeSimpleVertexShader();
		void initializeFontVertexShader();
		void initializeWorldFontVertexShader();
		void initializeParticleVertexShader();
		void initializeSimplePixelShader();
		void initializeSimpleMountainPixelShader();
		void initializeUIPixelShader();
		void initializeFontPixelShader();
		void initializeTreePixelShader();
		void initializeParticlePixelShader();
		void initializeTerrainVertexShader();
		void initializeInstancedFoliageVertexShader();
		void initializeFoliagePixelShader();
		void initializeWireFrameVertexShader();
		void initializeWireFramePixelShader();
		void initializeSkyVertexShader();
		void initializeSkyPixelShader();
		void initializeWaterPixelShader();
		void initializeBlurShader();
		void initializeBloomCombineShader();

	public:
		//Model Initialization
		
		ModelBufferData* CreateStandardBufferParticleSystemData(int maxConcurrentParticleCount) const;
		Model* CreateStandardParticleSystemModel(int maxConcurrentParticleCount) const;

		void CreateTextureFromArray(unsigned int* colors, unsigned int size, TextureData* textureDataResult) const;
		void UpdateTextureFromArray(unsigned int* colors, unsigned int size, TextureData* textureData) const;

		void UpdateBufferDataFromMesh(ModelBufferData* bufferData, const Geometry::LSystemMesh* mesh) const;
		void UpdateBufferFromParticleSystem(ModelBufferData* bufferData, ParticleSystem* particleSystem) const;
		void UpdateInstanceBufferData(InstanceBufferData* instanceBufferdata, DirectX::XMMATRIX* transforms) const;
		InstanceBufferData* CreateInstanceBufferData(unsigned int instanceCount = 1000) const;

		//Model Parsing
		float ChangePerlinAmplitude(float delta);

		int IncreaseTextureRepeatCount();
		int DecreaseTextureRepeatCount();

	};
}