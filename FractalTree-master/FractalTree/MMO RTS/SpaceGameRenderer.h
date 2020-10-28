#pragma once
#include "..\Common Source\Rendering Engine\Rendering Engine Sample\SampleRenderer.h"

namespace SpaceGameCore
{
	class SpaceGameRenderer : public RenderingEngine::SampleRenderer
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
			BloomCombineShader
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
		virtual void InitializeOther() override;
		virtual void InitializeShaders() override;
		virtual void InitializeTextures() override;

	private:
		bool initializeModels();

		void initializeSimpleVertexShader();
		void initializeRTSTerrainVertexShader();
		void initializeFontVertexShader();
		void initializeWorldFontVertexShader();
		void initializeSimplePixelShader();
		void initializeRTSTerrainPixelShader();
		void initializeUIPixelShader();
		void initializeFontPixelShader();
	};
}
