#pragma once
#ifndef _STRATEGY_GAME
#ifndef _WIN8_1

#ifndef _RTS_GAME
#include "BoundingBox.h"
#else

#include "..\Src\BoundingBox.h"
#endif

#else
#include "..\Src\BoundingBox.h"

#endif
#endif

#include <vector>

namespace RenderingEngine
{
	class SceneNode;

	class SceneGraph
	{
	private:
		std::vector<SceneNode*> nodes;
		int nodeCount;

		std::vector<SceneNode*> removeList;

		DirectX::XMVECTOR cameraLocation;
		DirectX::XMVECTOR cameraFacing;
		DirectX::XMVECTOR cameraUp;
		DirectX::XMVECTOR lightPosition;
		DirectX::XMVECTOR lightColor;
		DirectX::XMVECTOR light1Position;
		DirectX::XMVECTOR light1Color;
		DirectX::XMVECTOR sunPosition;
		DirectX::XMVECTOR sunColor;
		DirectX::XMMATRIX projectionMatrix;
		DirectX::XMVECTOR cameraOrientation;
		Geometry::Frustum viewFrustum;
		DirectX::XMVECTOR customValue;

	public:
		SceneGraph();
		void Update();
		void AddNode(SceneNode* node);
		void RequestNodeRemoval(SceneNode* node);
		void Reset();
		void DeleteRemovedNodes();
		//SetProjectionMatrix will also compute the frustum.
		void SetProjectionMatrix(const DirectX::XMMATRIX& projectionMatrix);
		~SceneGraph();

	private:
		void RemoveNode(SceneNode* node);
		void updateViewFrustum();

	public:
		const SceneNode* GetNode(unsigned int index) const {return this->nodes[index];}
		unsigned int GetNodeCount() const {return this->nodeCount;}
		void SetCameraLocation(const DirectX::XMVECTOR& loc) {this->cameraLocation = loc;}
		void SetCameraFacing(const DirectX::XMVECTOR& facing) {this->cameraFacing = facing;}
		void SetCameraOrientation(const DirectX::XMVECTOR& orientation) {this->cameraOrientation = orientation;}
		void SetCameraUp(const DirectX::XMVECTOR& up) {this->cameraUp = up;}
		void SetLightPosition(const DirectX::XMVECTOR& pos) {this->lightPosition = pos;}
		void SetLightColor(const DirectX::XMVECTOR& color) { this->lightColor = color; }
		void SetLight1Position(const DirectX::XMVECTOR& pos1) { this->light1Position = pos1; }
		void SetLight1Color(const DirectX::XMVECTOR& color1) { this->light1Color = color1; }
		void SetSunPosition(const DirectX::XMVECTOR& pos) { this->sunPosition = pos; }
		void SetSunColor(const DirectX::XMVECTOR& sunColor) { this->sunColor = sunColor; }
		void SetCustomValue(const DirectX::XMVECTOR& customValue) { this->customValue = customValue; }
		void SetViewFrustum(const Geometry::Frustum& viewFrustum) {this->viewFrustum = viewFrustum;}
		DirectX::XMVECTOR GetCameraLocation() const {return this->cameraLocation;}
		DirectX::XMVECTOR GetCameraFacing() const {return this->cameraFacing;}
		DirectX::XMVECTOR GetCameraOrientation() const {return this->cameraOrientation;}
		DirectX::XMVECTOR GetCameraUp() const {return this->cameraUp;}
		DirectX::XMVECTOR GetLightPosition() const {return this->lightPosition;}
		DirectX::XMVECTOR GetLightColor() const { return this->lightColor; }
		DirectX::XMVECTOR GetLight1Position() const { return this->light1Position; }
		DirectX::XMVECTOR GetLight1Color() const { return this->light1Color; }
		DirectX::XMVECTOR GetSunPosition() const { return this->sunPosition; }
		DirectX::XMVECTOR GetSunColor() const { return this->sunColor; }
		DirectX::XMVECTOR GetCustomValue() const { return this->customValue; }
		Geometry::Frustum GetViewFrustum() const {return this->viewFrustum;}
		DirectX::XMMATRIX GetProjectionMatriox() const { return this->projectionMatrix; }
	};
}