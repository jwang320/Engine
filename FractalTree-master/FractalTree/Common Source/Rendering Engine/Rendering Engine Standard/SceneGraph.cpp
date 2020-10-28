#include "pch.h"
#include "SceneGraph.h"
#include "SceneNode.h"
#include <DirectXMath.h>

using namespace DirectX;

namespace RenderingEngine
{
	SceneGraph::SceneGraph() :
		nodeCount(0),
		cameraLocation(XMVectorSet(0, 0, 0, 0)),
		cameraFacing(XMVectorSet(0, 0, 1, 0)),
		cameraUp(XMVectorSet(0, 1, 0, 0)),
		lightColor(XMVectorSet(1, 1, 1, 1)),
		lightPosition(XMVectorSet(0, 100, 300, 0)),
		light1Color(XMVectorSet(1, 1, 1, 1)),
		light1Position(XMVectorSet(0, 100, 300, 0)),
		sunPosition(XMVectorSet(0, 1000, 0, 1)),
		sunColor(XMVectorSet(2, 2, 1, 1)),
		customValue(XMVectorSet(0, 0, 0, 0))
	{

	}

	void SceneGraph::Update()
	{
		for(int a = 0; a < this->nodeCount; ++a)
		{
			if (this->nodes[a]->GetParentNode() == nullptr)
			{
				this->nodes[a]->Update();
			}
		}
	}

	void SceneGraph::SetProjectionMatrix(const XMMATRIX& projectionMatrix)
	{
		this->projectionMatrix = projectionMatrix;
	}

	void SceneGraph::updateViewFrustum()
	{
		this->viewFrustum.center = this->cameraLocation;
	}

	void SceneGraph::AddNode(SceneNode* node)
	{
		if(this->nodeCount >= this->nodes.size())
		{
			this->nodes.push_back(node);
			this->nodes[this->nodeCount]->SetNodeID(this->nodeCount);
			++this->nodeCount;
		}
		else
		{
			this->nodes[this->nodeCount] = node;
			this->nodes[this->nodeCount]->SetNodeID(this->nodeCount);
			++this->nodeCount;
		}
	}

	void SceneGraph::RequestNodeRemoval(SceneNode* node)
	{
		this->removeList.push_back(node);
	}

	void SceneGraph::RemoveNode(SceneNode* node)
	{
		//Keep track of parents in chain being deleted in this operation.
		node->SetIsBeingDeleted(true);

		const int childCount = node->GetChildCount();
		for(int a = 0; a < childCount; ++a)
		{
			this->RemoveNode(node->GetChildren()[a]);
		}

		//If the parent node is going to be deleted anyway (in the case of removing the entire tree),
		//it is not necessary to do this step.
		if(node->GetParentNode() != nullptr && !node->GetParentNode()->IsBeingDeleted())
		{
			node->GetParentNode()->RemoveChildNode(node);
		}

		const int tempID = node->GetNodeID();
		--this->nodeCount;
		delete node;

		if(this->nodeCount > 1)
		{
			if(tempID < this->nodeCount)
			{
				this->nodes[tempID] = this->nodes[this->nodeCount];
				this->nodes[tempID]->SetNodeID(tempID);
			}
		}

		this->nodes[this->nodeCount] = nullptr;
	}

	void SceneGraph::DeleteRemovedNodes()
	{
		if(this->removeList.size() > 0)
		{
			for(unsigned int a = 0; a < this->removeList.size(); ++a)
			{
				this->RemoveNode(this->removeList[a]);
			}

			this->removeList.clear();
		}
	}

	void SceneGraph::Reset()
	{
		for(unsigned int a = 0; a < this->GetNodeCount(); ++a)
		{
			if (this->nodes[a]->GetParentNode() == nullptr)
			{
				delete this->nodes[a];
			}
		}

		this->nodes.clear();
		this->nodes.shrink_to_fit();
		this->nodeCount = 0;
	}

	SceneGraph::~SceneGraph()
	{
		this->Reset();
	}
}