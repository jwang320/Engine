#include "pch.h"
#include "ModelBufferData.h"

namespace RenderingEngine
{
	ModelBufferData::ModelBufferData(int indexBufferCount) :
		indexBufferCount(indexBufferCount),
		indexBuffer(new ID3D11Buffer*[indexBufferCount]),
		indexCount(new unsigned int[indexBufferCount]),
		lodGridFactor(new int[indexBufferCount]),
		lodGridWidth(new int[indexBufferCount]),
		lodGridLength(new int[indexBufferCount])

	{
		
	}

	ModelBufferData::~ModelBufferData()
	{
		this->ReleaseBuffers();

		delete[] this->indexBuffer;
		delete[] this->indexCount;
		delete[] this->lodGridLength;
		delete[] this->lodGridFactor;
		delete[] this->lodGridWidth;		
	}

	void ModelBufferData::ReleaseBuffers()
	{
		this->ReleaseVertexBuffers();
		this->ReleaseIndexBuffers();
	}

	void ModelBufferData::ReleaseVertexBuffers()
	{
		this->vertexBuffer->Release();
	}

	void ModelBufferData::ReleaseIndexBuffers()
	{
		for (int a = 0; a < this->indexBufferCount; ++a)
		{
			this->indexBuffer[a]->Release();
		}
	}

	Model::~Model()
	{
		if(this->deleteBuffer)
		{
			delete this->bufferData;
		}

		if(this->instanceBufferData != NULL)
		{
			delete this->instanceBufferData;
		}
	}

	InstanceBufferData::InstanceBufferData()
	{

	}

	InstanceBufferData::~InstanceBufferData()
	{
		this->instanceBuffer->Release();
	}
}
