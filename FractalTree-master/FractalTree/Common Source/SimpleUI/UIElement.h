#pragma once
#include <string>
#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
#include <map>
#include <string>

namespace SimpleUI
{
	class Window;
	class UIElement;

	typedef std::function<void(UIElement* callingElement, int activationMethod, Window* owner)> OnUseCallback;
	typedef std::function<bool(UIElement*, Window*, short, float, float)> OnPointerPressCallback;

	static std::atomic_int elId(0);
	struct UILabel
	{
		std::string text;
		DirectX::XMVECTOR offset;
		DirectX::XMVECTOR scale = DirectX::XMVectorSet(.5f, .5f, 1, 1);
		bool visible = true;
	};

	class UIElement
	{
	protected:
		UIElement* parentElement = nullptr;
	private:
		bool isFocusable;
		const bool isRenderable;

		bool hasFocus = false;
		bool isEnabled = true;
		bool isChildrenEnabled = true;
		bool isEditable = false;
		bool isVisible = true;
		bool isClickable;
		bool autoPassClick = false;

		DirectX::XMVECTOR position = DirectX::XMVectorSet(0, 0, 0, 1);

	protected:
		ID3D11ShaderResourceView* texture = nullptr;
		ID3D11Texture2D* tex = nullptr;
		Window* const parentWindow;
		std::vector<UIElement*> children;
		std::map<std::string, UIElement*> childrenMap;
		DirectX::XMVECTOR bounds = DirectX::XMVectorSet(.5f, .5f, 1, 1);

		DirectX::XMVECTOR currentForegroundTint = DirectX::XMVectorSet(1, 1, 1, 1);
		DirectX::XMVECTOR foregroundTint = DirectX::XMVectorSet(1, 0, 0, 1);
		DirectX::XMVECTOR focusTint = DirectX::XMVectorSet(.55f, .55f, .55f, .55f);
		int editableStartIndex = 0;
		bool useMouse = false;

		OnUseCallback onUseCallback = nullptr;
		OnPointerPressCallback onClickCallback = nullptr;

		std::vector<UILabel> textLabels;
		UIElement* passThroughInputElement = nullptr;
		std::string name;
		int indexForParent = -1;

	public:
		UIElement(Window* parentWindow,
			OnUseCallback onUseCallback,
			bool isFocusable,
			bool isClickable,
			bool isRenderable);

		float rot = 0;
		unsigned int AddChild(UIElement* child, const std::string& childName = "");
		unsigned int AddTextLabel(const UILabel& label);
		void RemoveChild(UIElement* child);
		void Remove();
		//CheckPointerIntersection determines whether the pointer or mouse intersects
		//the UIElement.  First checks this element, then checks if child elements
		//intersect -- the return value is the deepest level child that intersects
		//the pointer, or NULL if none.
		UIElement* CheckPointerIntersection(const DirectX::XMVECTOR& point, const DirectX::XMMATRIX& parentTransformation, DirectX::XMFLOAT4& resultPoint);

		virtual ~UIElement() = 0;

		//Callbacks return true if the input is ultimately handled by this object.
		virtual void Use(int activationMethod);
		virtual bool OnKeyPressed(unsigned short whichKey, bool shiftDown = false);
		virtual bool OnPointerPress(unsigned char whichButton, float x, float y);
		virtual bool OnMouseMove(float x, float y) { return false; }
		void SetEnabled(bool isEnabled) {this->isEnabled = isEnabled;}
		void SetFocused(bool hasFocus) {this->hasFocus = this->isFocusable ? hasFocus : false;}
		void SetFocusTint(const DirectX::XMVECTOR& focusTint) { this->focusTint = focusTint; }
		void SetVisible(bool isVisible) {this->isVisible = isVisible;}
		void SetEditable(bool isEditable) { this->isEditable = isEditable; }
		void SetEditableStartIndex(int editableStartIndex) { this->editableStartIndex = editableStartIndex; }
		void SetClickable(bool clickable) { this->isClickable = clickable; }
		UIElement* SetOnUseCallback(OnUseCallback onUseCallback) { this->onUseCallback = onUseCallback; return this; }
		UIElement* SetOnClickCallback(OnPointerPressCallback onClickCallback) { this->onClickCallback = onClickCallback; return this; }
		void SetPosition(const DirectX::XMVECTOR& position) { this->position = DirectX::XMVectorSetW(position, 1); };
		void SetPosition(const float x, const float y) { this->SetPosition(DirectX::XMVectorSet(x, y, 0, 1)); };
		void SetBounds(const DirectX::XMVECTOR& bounds) {this->bounds = bounds;}
		void SetBounds(const float x, const float y) { this->SetBounds(DirectX::XMVectorSet(x, y, 1, 1)); }
		void SetForegroundTint(const DirectX::XMVECTOR& foregroundTint) {this->foregroundTint = foregroundTint; this->currentForegroundTint = foregroundTint;}
		void SetParent(UIElement* parent) {this->parentElement = parent;}
		void SetPassThroughInputElement(UIElement* passThroughInputElement) { this->passThroughInputElement = passThroughInputElement; }
		void SetFocusable(bool isFocusable) { this->isFocusable = isFocusable; }
		void SetAutoPassClick(bool autoPassClick) { this->autoPassClick = autoPassClick; }
		void ToggleFocus(const bool focus);

		bool IsClickable() const {return this->isClickable;}
		bool IsRenderable() const {return this->isRenderable;}
		bool AutoPassClick() const { return this->autoPassClick; }
		bool HasFocus() const {return this->hasFocus;}
		bool IsFocusable() const {return this->isFocusable;}
		bool IsEnabled() const {return this->isEnabled;}
		bool IsVisible() const {return this->isVisible;}
		bool IsEditable() const {return this->isEditable;}
		bool IsUsingMouse() const { return this->useMouse; }
		int GetIndexForParent() const { return this->indexForParent; }
		ID3D11ShaderResourceView* GetTexture() const {return this->texture;}

		UIElement* GetParent() const {return this->parentElement;}
		UIElement* GetPassThroughInputElement() const { return this->passThroughInputElement; }
		std::vector<UIElement*>& GetChildren(){return this->children;}
		UIElement* GetChild(const std::string& childName) { return this->childrenMap.find(childName) != this->childrenMap.end() ? this->childrenMap[childName] : NULL; }
		DirectX::XMVECTOR GetPosition() const {return this->position;}
		
		DirectX::XMMATRIX GetTransform() const {
			using namespace DirectX;
			const XMMATRIX myTransform = XMMatrixMultiply(XMMatrixScalingFromVector(DirectX::XMVectorSetZ(DirectX::XMVectorSetW(this->bounds, 1), 1)), XMMatrixTranslationFromVector(this->position));
			return this->parentElement != nullptr ? XMMatrixMultiply(myTransform, this->parentElement->GetTransform()) : myTransform;
		}
		DirectX::XMVECTOR GetBounds() const {return DirectX::XMVectorSetW(DirectX::XMVectorSetZ(this->bounds, 1), 1);}
		DirectX::XMVECTOR GetComposedBounds() const {
			if (this->parentElement == nullptr)
				return this->GetBounds();

			return DirectX::XMVectorMultiply(this->GetBounds(), this->parentElement->GetComposedBounds());
		}
		std::string GetName() const { return this->name; }
		std::map<std::string, UIElement*>& GetChildrenMap() { return this->childrenMap; }

		
		DirectX::XMVECTOR GetForegroundTint() const {return this->currentForegroundTint;}
		DirectX::XMVECTOR GetFocusTint() const {return this->focusTint;}
		int GetEditableStartIndex() const { return this->editableStartIndex; }
		auto GetOnUseCallback() const {return this->onUseCallback;}

		std::vector<UILabel>& GetTextLabels() { return this->textLabels; }
		const std::vector<UILabel>& GetTextLabels() const { return this->textLabels; }

		template <typename NumberType>
		NumberType getInputNumber() const
		{
			using namespace std;
			const string s = this->GetTextLabels()[0].text.substr(this->GetEditableStartIndex());
			stringstream strm(s);
			NumberType m;
			strm >> m;
			return m;
		}
#pragma endregion
	};
}