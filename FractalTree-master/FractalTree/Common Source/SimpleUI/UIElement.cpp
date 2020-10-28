#include "pch.h"
#include "UIElement.h"
#include "Window.h"

using namespace DirectX;

namespace SimpleUI
{
	UIElement::UIElement(Window* parentWindow,
		OnUseCallback onUseCallback,
		bool isFocusable,
		bool isClickable,
		bool isRenderable) : 
		parentWindow(parentWindow),
		isFocusable(isFocusable),
		bounds(bounds),
		onUseCallback(onUseCallback),
		isClickable(isClickable),
		isRenderable(isRenderable)
	{
		this->name = std::to_string(elId++);
	}

	UIElement::~UIElement()
	{

	}

	UIElement* UIElement::CheckPointerIntersection(const XMVECTOR& point, const XMMATRIX& parentTransformation, XMFLOAT4& resultPoint)
	{
		using namespace DirectX;
		XMFLOAT4 p;
		const XMMATRIX myTransform = XMMatrixMultiply(XMMatrixMultiply(
			XMMatrixScalingFromVector(XMVectorSetW(XMVectorSetZ(this->bounds, 1), 1)),
			//XMMatrixIdentity(),
			//XMMatrixRotationRollPitchYawFromVector(this->pitchRollYaw)), 
			//XMMatrixRotationRollPitchYawFromVector(XMVectorSet(0, 0, element->rot, 1))),
			XMMatrixIdentity()),
			XMMatrixTranslationFromVector(XMVectorSetW(this->position, 1)));
		

		//XMVECTOR boundsToUse = this->GetComposedBounds();//this->useRelativeScale ? XMVectorMultiply(this->bounds, this->parentElement->bounds) : this->bounds;
		XMVECTOR boundsToUse = this->bounds ;
		//XMVECTOR halfBounds = XMVectorMultiply(boundsToUse, XMVectorSet(this->parentWindow->GetWidth() * .25f, this->parentWindow->GetHeight() * .25f, 0, 0));
		XMVECTOR halfBounds = XMVectorSet(.5,.5, 0, 0);
		
		//const XMMATRIX finalTransform = XMMatrixMultiply(XMMatrixScalingFromVector(XMVectorSet(1, 1, 1, 1)), this->GetTransform());
		XMMATRIX finalTransform = XMMatrixMultiply(myTransform, parentTransformation);
		finalTransform = XMMatrixMultiply(XMMatrixScalingFromVector(XMVectorSet(1, 1, 1, 1)), finalTransform);
		const XMMATRIX inverseTransform = XMMatrixInverse(nullptr, finalTransform);

		XMStoreFloat4(&p, XMVector3Transform(point, inverseTransform));
		const XMVECTOR realPosition = XMVectorZero();
		XMFLOAT4 bottomLeft;
		XMStoreFloat4(&bottomLeft, XMVectorSubtract(realPosition, halfBounds));

		XMFLOAT4 topRight;
		XMStoreFloat4(&topRight, XMVectorAdd(realPosition, halfBounds));

		//Check if point lies within this element's bounds.
		//if(XMVector2LessOrEqual(point, XMVectorAdd(this->position, this->bounds)) && XMVector2GreaterOrEqual(point, XMVectorSubtract(this->position, this->bounds)))
		if (this->autoPassClick || p.x > bottomLeft.x && p.x < topRight.x && p.y > bottomLeft.y && p.y < topRight.y)
		{
			//Give all children precedence for intersection
			for(auto c : this->childrenMap)
			{
				//Transform the point to local coordinates of this UIElement
				//XMVECTOR transformedPoint = XMVectorMultiply(point, XMVectorMultiply(this->bounds, XMVectorReplicate(.5f)));
				//UIElement* child = this->children[a]->CheckPointerIntersection(XMVectorSubtract(transformedPoint, this->position));
				UIElement* child = c.second->CheckPointerIntersection(point, finalTransform, resultPoint);
				if(child != NULL)
				{
					return child;
				}
			}

			if(this->IsClickable() && p.x > bottomLeft.x && p.x < topRight.x && p.y > bottomLeft.y && p.y < topRight.y)
			{
				resultPoint.x = p.x;
				resultPoint.y = p.y;
				return this;
			}
		}

		return NULL;
	}

	//returns the index of the added element.
	unsigned int UIElement::AddChild(UIElement* child, const std::string& childName)
	{
		this->children.push_back(child); 
		if (childName != "")
		{
			this->childrenMap[childName] = child;
			child->name = childName;
		}
		else
		{
			this->childrenMap[child->name] = child;
		}

		child->SetParent(this);
		child->indexForParent = static_cast<unsigned int>(this->children.size()) - 1;
		return child->indexForParent;
	}

	unsigned int UIElement::AddTextLabel(const UILabel& label)
	{
		this->textLabels.push_back(label);
		return static_cast<unsigned int>(this->textLabels.size()) - 1;
	}

	void UIElement::RemoveChild(UIElement * child)
	{
		if (auto i = this->childrenMap.find(child->name); i != this->childrenMap.end())
		{
			this->childrenMap.erase(i);
		}

		delete child;
	}

	void UIElement::Remove()
	{
		if (this->parentElement != nullptr)
		{
			this->parentElement->RemoveChild(this);
		}
	}

	bool UIElement::OnKeyPressed(unsigned short whichKey, bool shiftDown)
	{
		//if (this->focu)
		return false;
	}

	bool UIElement::OnPointerPress(unsigned char whichButton, float x, float y)
	{
		if(this->onClickCallback != NULL)
		{
			return this->onClickCallback(this, this->parentWindow, whichButton, x, y);
		}

		return false;
	}

	void UIElement::ToggleFocus(const bool focus)
	{
		if (this->hasFocus)
		{
			//this->parentElement->foc
		}
	}

	void UIElement::Use(int activationMethod)
	{
		this->onUseCallback(this, activationMethod, this->parentWindow);
	}
}
