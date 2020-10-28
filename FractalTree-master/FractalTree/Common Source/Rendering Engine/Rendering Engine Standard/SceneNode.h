#pragma once
#include "ModelNode.h"
#include "ParticleNode.h"
#include <DirectXMath.h>
#include <map>
#include <string>

namespace RenderingEngine
{
	class SceneEntity;
	class ParticleEntity;
	class SceneGraph;
	namespace Animations
	{
		struct Animation;
		struct AnimationState;
	}

	enum ActionTarget
	{
		TSelf,
		TModelEntity,
		TParticleEntity
	};

	enum TransformTarget
	{
		TLocation,
		TOrientation,
		TScale
	};

	//A scene node in the scene graph can contain a SceneEntity, which contain rendering information
	class SceneNode : public ModelNode, public ParticleNode
	{
	private:
		SceneGraph* parentGraph;
		SceneNode* parentNode;
		std::vector<SceneNode*> children;
		int childCount;
		int life;
		bool timed;

		DirectX::XMVECTOR translation;
		//DirectX::XMVECTOR pitchRollYaw;
		DirectX::XMVECTOR qRotation;
		DirectX::XMVECTOR scale;

		DirectX::XMMATRIX transformation;
		DirectX::XMVECTOR tint;
		DirectX::XMMATRIX absoluteTransformation;
		DirectX::XMVECTOR velocity;
		DirectX::XMVECTOR localScale = DirectX::XMVectorSet(1, 1, 1, 0);
		DirectX::XMVECTOR localTranslation = DirectX::XMVectorSet(0, 0, 0, 1);
		std::vector<Animations::Animation*> animations;
		int nodeID;
		bool removed = false;
		bool isBeingDeleted = false;
		bool isVisible = true;
		std::map<std::string, SceneNode*> namedChildren;
		std::string name;
		bool useManualTransform = false;
		DirectX::XMMATRIX manualtransform = DirectX::XMMatrixIdentity();

	public:
		SceneNode(SceneGraph* parentGraph, bool timed = false, int life = 0);
		void Update();
		void AddChildNode(SceneNode* node);
		void AddChildNode(SceneNode* node, const std::string& childName);
		SceneNode* GetChildByName(const std::string& name) {return this->namedChildren[name];}
		void RemoveChildNode(SceneNode* node);
		void RemoveChildren();
		void Remove();
		~SceneNode();

	private:
		void SetParentNode(SceneNode* const parentNode) { this->parentNode = parentNode; }

	public:
#pragma region Getters and Setters
		void SetUseManualTransform(const bool useManualTransform) { this->useManualTransform = useManualTransform; }
		void SetManualTransform(const DirectX::XMMATRIX& transform) { this->manualtransform = transform; }
		SceneNode* GetParentNode() const {return this->parentNode;}
		const std::vector<SceneNode*>& GetChildren() const {return this->children;}
		DirectX::XMMATRIX GetTransformation() const {return this->transformation;}
		DirectX::XMVECTOR GetTint() const {return this->tint;}
		DirectX::XMMATRIX GetAbsoluteTransformation() const {return this->absoluteTransformation;}
		int GetNodeID() const {return this->nodeID;}
		int GetChildCount() const {return this->childCount;}
		DirectX::XMVECTOR GetTranslation() const {return this->translation;}
		//DirectX::XMVECTOR GetPitchRollYaw() const {return this->pitchRollYaw;}
		DirectX::XMVECTOR GetScale() const {return this->scale;}
		DirectX::XMVECTOR GetVelocity() const {return this->velocity;}
		DirectX::XMVECTOR GetLocalScale() const { return this->localScale; }
		DirectX::XMVECTOR GetLocalTranslation() const { return this->localTranslation; }
		bool IsBeingDeleted() const { return this->isBeingDeleted; }
		bool Removed() const { return this->removed; }
		bool IsVisible() const { return this->isVisible; }

		void SetTransformationVector(const DirectX::XMVECTOR& scale, const DirectX::XMVECTOR& pitchRollYaw, const DirectX::XMVECTOR& translation) {
			this->scale = scale;
			//this->pitchRollYaw = pitchRollYaw,
			this->qRotation = pitchRollYaw;
				this->translation = translation;}
		void SetTranslation(const DirectX::XMVECTOR& translation) {this->translation = translation;}
		//void SetPitchRollYaw(const DirectX::XMVECTOR& pitchRollYaw) {this->pitchRollYaw = pitchRollYaw;}
		void SetScale(const DirectX::XMVECTOR& scale) {this->scale = scale;}
		void SetTransformation(const DirectX::XMMATRIX& transformation) {this->transformation = transformation;}
		void SetTint(const DirectX::XMVECTOR& tint) {this->tint = tint;}
		void SetAbsoluteTransformation(const DirectX::XMMATRIX& absoluteTransformation) {this->absoluteTransformation = absoluteTransformation;}
		void SetNodeID(int nodeID) {this->nodeID = nodeID;}
		void SetVelocity(const DirectX::XMVECTOR& velocity) {this->velocity = velocity;}
		void SetIsBeingDeleted(bool isBeingDeleted) { this->isBeingDeleted = isBeingDeleted; }
		void SetLocalScale(const DirectX::XMVECTOR& localScale) { this->localScale = localScale; }
		void SetLocalTranslation(const DirectX::XMVECTOR& localTranslation) { this->localTranslation = localTranslation; }
		void SetVisible(bool isVisible) {
			this->isVisible = isVisible; for (int a = 0; a < this->childCount; ++a) { this->children[a]->SetVisible(isVisible); }
		}
		void SetName(const std::string& name) { this->name = name; }
#pragma endregion
	};
}