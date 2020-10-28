#pragma once
#include "..\Common Source\SimpleUI\UIContainer.h"

namespace SimulationEngine { class DynamicUserOfSkeleton;}

namespace InverseKinematics { class SkeletonSegment; }

namespace SimpleUI
{
	class UIButton;
	class UIImage;
	class UIShape;
	class UILine;

	struct RobotDot
	{
		DirectX::XMVECTOR pos = DirectX::XMVectorZero();
		UIShape* shape = nullptr;
		InverseKinematics::SkeletonSegment* segment = nullptr;
		UIShape* connectShape = nullptr;
		UILine* connectLine = nullptr;
		float ang = 0;
		int ind;
		DirectX::XMFLOAT4 minValues = { -0.785f, -.785f, -0.785f, -0.785f };
		DirectX::XMFLOAT4 maxValues = { 0.785f, 0.785f, 0.785f, 0.785f };
		RobotDot* parent = nullptr;
		float len = 0;

		float getTotalZRot();
	};

	class UIShape : public UIElement
	{
	public:
		RobotDot* const dot;
		UILabel* xLabel = nullptr;
		UILabel* yLabel = nullptr;
		UILabel* zLabel = nullptr;
		UILabel* defaultZLabel = nullptr;

	public:
		UIShape(Window* parentWindow, const DirectX::XMVECTOR& pos, RobotDot* dot, float rot);
		void makeBig();
		void makeSmall();
		void updateLimitsText();
	};

	class UILine : public UIElement
	{
	public:
	public:
		UILine(Window* parentWindow, const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end);
	};
	class UIPane : public UIElement
	{
	public:
		UIPane(Window* parentWindow);
	};

	class UIMapEditor : public UIControlContainer
	{
	private:
		UIImage* previewPane = nullptr;
		UIButton* sizeButton = nullptr;
	public:
		std::vector<RobotDot*> robotDots = std::vector<RobotDot*>();
		SimulationEngine::DynamicUserOfSkeleton* skeleton = nullptr;
		RobotDot* selectedDot = nullptr;
		int selectIndex = 0;
		float totalZRot = 0;
		UIButton* editXMinButton = nullptr;
		UIButton* editXMaxButton = nullptr;
		UIButton* editYMinButton = nullptr;
		UIButton* editYMaxButton = nullptr;
		UIButton* editZMinButton = nullptr;
		UIButton* editZMaxButton = nullptr;
		UIButton* editZRotButton = nullptr;
		UIButton* isGoalButton = nullptr;
		UIButton* isFootButton = nullptr;
		UIButton* isAngleGoalButton = nullptr;
		UIButton* disableConstraintsButton = nullptr;
		int buttonCount = 0;
		const float startX = .35f;
		const float startY = .48f;
		const float yDelta = .032f;
		const float xSize = .2f;
		const float ySize = .025f;
		const float textX = -.5f;
		const float textY = -.5f;
		float scale = 140.f;
		float zoom = 1.f;
		bool constraintsDiabled = false;
	public:
		UIMapEditor(Window* parentWindow, const UIContainerLayout& layout);
		virtual bool OnKeyPressed(unsigned short whichKey, bool shiftDown = false) override;
		void deleteLast();
		~UIMapEditor();

	private:
		void createGlobalSettingsButtons();
		void createSegmentEditorPane();
		void createRobotBuildingPane();
		void selectDot(RobotDot* dot);
		void setScale(const float scale);
		void resetDots();
	};
}