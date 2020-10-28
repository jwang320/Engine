#include "pch.h"
#include "SceneNode.h"
#include "SceneGraph.h"
#include "ParticleEntity.h"

using namespace RenderingEngine::Animations;
using namespace DirectX;

namespace RenderingEngine
{
	class SceneNode;

	SceneNode::SceneNode(SceneGraph* parentGraph, bool timed, int life) :
		parentGraph(parentGraph),
		transformation(XMMatrixIdentity()),
		absoluteTransformation(XMMatrixIdentity()),
		translation(XMVectorSet(0, 0, 0, 1)),
		//pitchRollYaw(XMVectorSet(0, 0, 0, 0)),
		qRotation(XMQuaternionIdentity()),
		scale(XMVectorSet(1, 1, 1, 1)),
		childCount(0),
		parentNode(nullptr),
		timed(timed),
		life(life),
		velocity(XMVectorSet(0, 0, 0, 0))
	{
		this->parentGraph->AddNode(this);
	}

	//Recursively update this scene node and its children.
	void SceneNode::Update()
	{
		if (!this->useManualTransform)
		{
			this->transformation = XMMatrixMultiply(XMMatrixMultiply(
				XMMatrixScalingFromVector(this->scale),
				//XMMatrixRotationRollPitchYawFromVector(this->pitchRollYaw)), 
				XMMatrixRotationQuaternion(this->qRotation)),
				XMMatrixTranslationFromVector(this->translation));
		}
		else
		{
			this->transformation = this->manualtransform;
		}

		if(this->parentNode != nullptr)
		{
			this->absoluteTransformation = XMMatrixMultiply(this->transformation, this->parentNode->GetAbsoluteTransformation());
		}
		else
		{
			this->absoluteTransformation = this->transformation;
		}

		this->UpdateParticleNode();

		for(int a = 0; a < this->childCount; ++a)
		{
			this->children[a]->Update();
		}

		this->absoluteTransformation = XMMatrixMultiply(XMMatrixTranslationFromVector(this->localTranslation), this->absoluteTransformation);
		this->absoluteTransformation = XMMatrixMultiply(XMMatrixScalingFromVector(this->localScale), this->absoluteTransformation);
		this->DeleteRemovedParticleEntities();
		this->DeleteRemovedModelEntities();

		if(this->timed)
		{
			if(this->life < 0)
			{
				this->Remove();
			}
			else
			{
				this->life--;
			}
		}
	}

	void SceneNode::AddChildNode(SceneNode* node)
	{
		node->SetParentNode(this);
		if(this->childCount >= this->children.size())
		{
			this->children.push_back(node);
			++this->childCount;
		}
		else
		{
			this->children[this->childCount] = node;
			++this->childCount;
		}
	}

	void SceneNode::AddChildNode(SceneNode* node, const std::string& childName)
	{
		this->AddChildNode(node);
		this->namedChildren[childName] = node;
		node->SetName(childName);
	}

	void SceneNode::RemoveChildNode(SceneNode* node)
	{
		for(int a = 0; a < this->childCount; ++a)
		{
			if(this->children[a] == node)
			{
				--this->childCount;
				if(a < this->childCount)
				{
					this->children[a] = this->children[this->childCount];
				}

				this->children[this->childCount] = NULL;
			}
		}
	}

	void SceneNode::RemoveChildren()
	{
		for (int a = 0; a < this->childCount; ++a)
		{
			this->children[a]->Remove();
			this->children[a] = nullptr;
		}

		this->childCount = 0;
	}

	void SceneNode::Remove()
	{
		if(!this->removed)
		{
			this->parentGraph->RequestNodeRemoval(this);
			if (this->parentNode != nullptr && !this->name.empty())
			{
				this->parentNode->namedChildren[this->name] = nullptr;
			}
			this->removed = true;
		}
	}

	SceneNode::~SceneNode()
	{
	}
}