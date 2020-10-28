#include "pch.h"
#include "UIImage.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Base\Direct3DBase.h"

using namespace RenderingEngine;

namespace SimpleUI
{
	UIImage::UIImage(Window* parentWindow) :
		UIElement(parentWindow, nullptr, false, true, true)
	{
		
	}

	void UIImage::Init(int imageWidth, int imageHeight)
	{
		if(this->texture != NULL)
		{
			if(this->imageWidth == imageWidth && this->imageHeight == imageHeight)
			{
				return;
			}

			this->texture->Release();
			this->tex->Release();
		}

		this->imageWidth = imageWidth;
		this->imageHeight = imageHeight;

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = this->imageWidth;
		textureDesc.Height = this->imageHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Format =  DXGI_FORMAT_R10G10B10A2_UNORM;
		textureDesc.Usage = D3D11_USAGE_DYNAMIC;

		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//ID3D11Texture2D* tex = NULL;
		//textureDataResult->texture = tex;
		if(FAILED(RenderDevices::device->CreateTexture2D(&textureDesc, NULL, &this->tex)))
		{
			throw int();
		}


		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
		srDesc.Format = textureDesc.Format;
		srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc.Texture2D.MostDetailedMip = 0;
		srDesc.Texture2D.MipLevels = 1;

		if(FAILED(RenderDevices::device->CreateShaderResourceView(this->tex, &srDesc, &this->texture)))
		{
			throw int();
		}
	}

	void UIImage::RefreshImage(unsigned int* image)
	{
		D3D11_MAPPED_SUBRESOURCE ms;
		RenderDevices::deviceContext->Map(this->tex, 0, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

		memcpy(ms.pData, image, sizeof(unsigned int) * this->imageWidth * this->imageHeight);
		RenderDevices::deviceContext->Unmap(this->tex, 0);
	}

	UIImage::~UIImage()
	{

	}
}