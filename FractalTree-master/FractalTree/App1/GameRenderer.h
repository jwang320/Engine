#pragma once
#include "..\Rendering Engine Sample\SampleRenderer.h"
#ifndef _WIN32_APPLICATION
#include <agile.h>
#endif
namespace RenderingEngine { class UIRenderer; class InstanceBufferData; struct ColoredInstanceVertex; }
namespace SimpleUI { class Window; }

//The GameRenderer class is a small extension to the main engine's renderer
//to handle custom resources needed for this specific program.  Most of the
//actual rendering code is in the base classes, and should not need to be touched.
class GameRenderer : public RenderingEngine::SampleRenderer
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
#ifndef _WIN32_APPLICATION
	Platform::Agile<Windows::UI::Core::CoreWindow> thisWindow;
#endif
#ifdef _WIN32_APPLICATION
	HWND thisWindow;
#endif

public:
	GameRenderer();
#ifndef _WIN32_APPLICATION
	void InitializeDevice(Platform::Agile<Windows::UI::Core::CoreWindow> window);
#endif
#ifdef _WIN32_APPLICATION
	void InitializeDevice2(HWND systemWindow, bool fullScreen = true, int width = 1600, int height = 900);
#endif

	void UpdateInstanceBufferData(RenderingEngine::InstanceBufferData* instanceBufferdata, RenderingEngine::ColoredInstanceVertex* transforms) const;
	RenderingEngine::InstanceBufferData* CreateInstanceBufferData(unsigned int instanceCount) const;
	~GameRenderer();

protected:
	virtual void InitializeOther() override;
	virtual void InitializeShaders() override;
	virtual void InitializeRasterizerStates() override;
	virtual void InitializeTextures() override;

private:
	void initializeSimpleVertexShader();
	void initializeSimpleInstancedVertexShader();
	void initializeFontVertexShader();
	void initializeSimplePixelShader();
	void initializeUIPixelShader();
	void initializeFontPixelShader();
};