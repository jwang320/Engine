#pragma once

namespace SimpleUI
{
	class UIElement;

	class Window
	{
	protected:

	private:
		bool keyDown[256];
		UIElement* foregroundUIContainer;
		float width;
		float height;
	public:
		float mouseX = 0;
		float mouseY = 0;
		bool mouseDown[2];
		bool showMouse = true;
		Window(UIElement* foregroundUIContainer = 0);

		//When overriding this function, it is imperative that the base class version be called from the overriden version.
		//Should return true if the input was handled.
		virtual bool SetKeyDown(unsigned short whichKey, bool shiftDown = false) = 0;
		virtual bool SetKeyUp(unsigned short whichKey);

		virtual bool SetPointerDown(unsigned char whichButton, float x, float y);
		bool mouseMove(float x, float y);
		virtual ~Window();
		float getAspectRatio() { return (float)this->width / this->height; }

	protected:
		void SetForegroundUIElement(UIElement* foregroundUIContainer);

	public:
#pragma region Getters and Setters
		UIElement* GetForegroundUIContainer() const {return this->foregroundUIContainer;}
		bool KeyDown(unsigned short whichKey) const {return this->keyDown[whichKey];}
		float GetWidth() const { return this->width; }
		float GetHeight() const { return this->height; }

		void SetBounds(float width, float height) {this->width = width; this->height = height;}
#pragma endregion
	};
}