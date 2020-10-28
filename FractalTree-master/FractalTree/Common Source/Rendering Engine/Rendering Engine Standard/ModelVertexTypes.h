#pragma once
#include <DirectXMath.h>

namespace RenderingEngine
{
	struct OncePerFrameConstantBuffer
	{
		DirectX::XMMATRIX view;
		DirectX::XMVECTOR eyePoint;
		DirectX::XMVECTOR light0Position;
		DirectX::XMVECTOR light0Color;
		DirectX::XMVECTOR sunDirection;
		DirectX::XMVECTOR sunColor;
		DirectX::XMVECTOR cameraUp;
		DirectX::XMMATRIX lightProjection;
		DirectX::XMMATRIX lightView;
		DirectX::XMVECTOR light1Position;
		DirectX::XMVECTOR light1Color;
		DirectX::XMVECTOR customValue;
	};

	struct DownScaleConstantsBuffer
	{
		unsigned int Res[2];
		unsigned int Domain;
		unsigned int GroupSize;
	};

	struct PerlinBuffer
	{
		float terrainTextureRepeatCount[4];
	};

	struct OncePerResizeConstantBuffer
	{
		DirectX::XMMATRIX projection;
	};

	struct OncePerModelConstantBuffer
	{
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX scaleMatrix;
		DirectX::XMVECTOR tint;
		DirectX::XMVECTOR diffuseTint;
		DirectX::XMVECTOR customValue; //anything can go here
	};

	struct OncePerModelConstantBufferShadow
	{
		DirectX::XMMATRIX model;
	};

	struct VertexColor
	{
		DirectX::XMVECTOR pos;
		DirectX::XMVECTOR color;
	};

	struct VertexPositionColor
	{
		DirectX::XMVECTOR pos;
		DirectX::XMVECTOR color;
		DirectX::XMVECTOR normal;
		DirectX::XMVECTOR tangent;
		DirectX::XMVECTOR binormal;
		DirectX::XMVECTOR emissiveColor;
		DirectX::XMVECTOR specularColor;
		DirectX::XMVECTOR ambientColor;
		DirectX::XMFLOAT2 texCoord;
		float specularExponent;
	};

	struct UIVertexPositionColor
	{
		DirectX::XMVECTOR position;
		float texCoord[2];
	};

	struct TexturedVertex
	{
		DirectX::XMVECTOR pos;
		DirectX::XMVECTOR normal;
		DirectX::XMVECTOR tangent;
		DirectX::XMVECTOR binormal;
		DirectX::XMFLOAT2 texCoord;
	};

	struct TexturedSimpleVertex
	{
		DirectX::XMVECTOR pos;
		DirectX::XMVECTOR normal;
		DirectX::XMFLOAT2 texCoord;
	};

	struct InstanceVertex
	{
		DirectX::XMMATRIX instanceTransform;
	};

	struct ColoredInstanceVertex
	{
		DirectX::XMMATRIX instanceTransform;
		DirectX::XMVECTOR diffuseColor;
		DirectX::XMVECTOR emissiveColor;
		float specularExponent;
	};
}