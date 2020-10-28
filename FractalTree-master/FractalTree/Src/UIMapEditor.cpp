#include "pch.h"
#include "UIMapEditor.h"
#include "FractalTreeWindow.h"
#include "UIImage.h"
#include <sstream>
#include "..\Common Source\SimpleUI\UIButton.h"
#include "UIRenderer.h"
#include "..\FractalTree Common\Game Objects\Objects\DynamicUserOfSkeleton.h"
#include "..\Common Source\Skeleton\Skeleton.h"
#include "..\FractalTree Common\FractalTreeSimulation.h"
#include "..\Common Source\Skeleton\SkeletonSegment.h"
#include "FractalTreeResources.h"

using namespace DirectX;
using namespace std;
using namespace SimulationEngine;
using namespace InverseKinematics;

namespace SimpleUI
{
	UIMapEditor::UIMapEditor(Window* parentWindow, const UIContainerLayout& layout) :
		UIControlContainer(parentWindow, layout)
	{
		this->SetClickable(true);
		this->useMouse = true;
		this->createSegmentEditorPane();
		this->createGlobalSettingsButtons();
		this->createRobotBuildingPane();
	}

	const auto clickCallback = [](UIElement* callingElement, Window* owner, short whichButton, float x, float y)
	{
		UIMapEditor* parent = dynamic_cast<UIMapEditor*>(callingElement->GetParent());
		if (callingElement->HasFocus())
			parent->SetFocusElement(-1);
		else
		{
			parent->SetFocusElement(callingElement->GetIndexForParent());
		}
		return true;
	};

	void UIMapEditor::createRobotBuildingPane()
	{
		this->previewPane = new UIImage(this->parentWindow);
		this->previewPane->SetBounds(XMVectorSet(.95f, .95f, 1, 1));
		this->previewPane->SetForegroundTint(XMVectorSet(0, 0, 0, 0));
		this->previewPane->SetPosition(XMVectorSet(0, 0, 0, 0));
		this->previewPane->SetClickable(true);
		this->previewPane->SetOnClickCallback([this](UIElement* callingElement, Window* parentWindow, short whichButton, float x, float y)
		{
			RobotDot* const newDot = new RobotDot();
			newDot->pos = XMVectorSet(x, y, 0, 1);

			newDot->ind = this->robotDots.size();
			if (this->constraintsDiabled)
			{
				newDot->minValues = { -10000, -10000, -10000, -10000 };
				newDot->maxValues = { 10000, 10000, 10000, 10000 };
			}
			this->selectIndex = newDot->ind;
			this->skeleton = static_cast<SimulationEngine::DynamicUserOfSkeleton*>(static_cast<FractalTreeWindow*>(parentWindow)->simulation->GetWorld()->testArm);
			if (!this->robotDots.empty()) {

				XMFLOAT4 lenV, oldPos, newPos;
				XMStoreFloat4(&lenV, XMVector2Length((XMVectorSubtract(newDot->pos, this->selectedDot->pos))));
				XMStoreFloat4(&oldPos, this->selectedDot->pos);
				XMStoreFloat4(&newPos, newDot->pos);
				const float len = this->scale * lenV.x;
				const float ang = fabs(newPos.y - oldPos.y) > .00001f ? atan2(newPos.x - oldPos.x, newPos.y - oldPos.y) : 0;
				newDot->segment = this->skeleton->MakeSegment("", "", len, false, this->robotDots.size() == 1, false,
					this->selectedDot->segment, true, XMVectorSet(0, 0, this->selectedDot->ang - ang, 0),
					newDot->minValues, newDot->maxValues);
				newDot->ang = ang;
				newDot->parent = this->selectedDot;
				newDot->len = len;
				UILine* const newLine = new UILine(parentWindow, this->selectedDot->pos, newDot->pos);
				callingElement->AddChild(newLine);
			}
			else {
				newDot->segment = this->skeleton->MakeSegment("root", "", 0, false, false, false, nullptr, false);
			}
			newDot->shape = new UIShape(parentWindow, XMVectorSet(x, y, 0, 1), newDot, 0);
			newDot->shape->SetForegroundTint(XMVectorSet(.2f, .2f, .2f, 1));
			newDot->shape->SetClickable(true);
			newDot->shape->SetOnClickCallback([this, newDot](UIElement* callingElement2, Window* parentWindow2, short whichButton2, float x2, float y2)
			{
				this->selectDot(newDot);
				return true;
			});
			this->robotDots.push_back(newDot);
			this->selectDot(newDot);
			callingElement->AddChild(newDot->shape);
			return true;
		});
		this->AddChild(previewPane);
	}

	void UIMapEditor::selectDot(RobotDot* dot)
	{
		if (this->selectedDot != nullptr)
		{
			this->selectedDot->shape->makeSmall();
		}
		this->selectedDot = dot;
		this->selectedDot->shape->makeBig();
		this->selectIndex = this->selectedDot->ind;
		const string xMinT = "X min angle: ";
		this->editXMinButton->GetTextLabels()[0].text = xMinT + to_string(dot->segment->minValues.x);
		this->editXMinButton->SetEditableStartIndex(xMinT.size());
		const string xMaxT = "X max angle: ";
		this->editXMaxButton->GetTextLabels()[0].text = xMaxT + to_string(dot->segment->maxValues.x);
		this->editXMaxButton->SetEditableStartIndex(xMaxT.size());

		const string yMinT = "Y min angle: ";
		this->editYMinButton->GetTextLabels()[0].text = yMinT + to_string(dot->segment->minValues.y);
		this->editYMinButton->SetEditableStartIndex(yMinT.size());
		const string yMaxT = "Y max angle: ";
		this->editYMaxButton->GetTextLabels()[0].text = yMaxT + to_string(dot->segment->maxValues.y);
		this->editYMaxButton->SetEditableStartIndex(yMaxT.size());

		const string zMinT = "Z min angle: ";
		this->editZMinButton->GetTextLabels()[0].text = zMinT + to_string(dot->segment->minValues.z);
		this->editZMinButton->SetEditableStartIndex(zMinT.size());
		const string zMaxT = "Z max angle: ";
		this->editZMaxButton->GetTextLabels()[0].text = zMaxT + to_string(dot->segment->maxValues.z);
		this->editZMaxButton->SetEditableStartIndex(zMaxT.size());

		XMFLOAT4 rot;
		XMStoreFloat4(&rot, dot->segment->defaultPitchYawRoll);
		const string z1 = "Z Rot: ";
		this->editZRotButton->GetTextLabels()[0].text = z1 + to_string(rot.z);
		this->editZRotButton->SetEditableStartIndex(z1.size());
		this->isGoalButton->GetTextLabels()[0].text = "Is goal: " + to_string(this->selectedDot->segment->hasDistanceGoal());
		this->isFootButton->GetTextLabels()[0].text = "Is foot: " + to_string(this->selectedDot->segment->hasHoldPositionGoal());
		this->isAngleGoalButton->GetTextLabels()[0].text = "Is vert ang: " + to_string(this->selectedDot->segment->hasAbsoluteFacingGoal());
	}

	void UIMapEditor::setScale(const float scale)
	{
		this->scale = scale;
		this->resetDots();
	}
	
	void UIMapEditor::resetDots()
	{
		for (RobotDot* const dot : this->robotDots)
		{
			if (dot->parent != nullptr)
			{
				XMFLOAT4 rot, lineLen;
				XMStoreFloat4(&rot, dot->segment->defaultPitchYawRoll);
				XMStoreFloat4(&lineLen, XMVector2Length(XMVectorSubtract(dot->shape->GetPosition(), dot->parent->shape->GetPosition())));
				const XMVECTOR newPos = XMVectorAdd(dot->parent->shape->GetPosition(),
					XMVector3Rotate(XMVectorSet(0, dot->len * this->zoom / this->scale, 0, 0),
						XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), dot->getTotalZRot())));
				dot->shape->SetPosition(newPos);
			}
		}
	}

	bool UIMapEditor::OnKeyPressed(unsigned short whichKey, bool shiftDown)
	{
		bool handled = __super::OnKeyPressed(whichKey, shiftDown);
		if (handled)
			return handled;

		if (whichKey == 'E')
		{
			this->deleteLast();
			return true;
		}
		else if (whichKey == 'I')
		{
			this->selectDot(this->robotDots[this->selectIndex - 1]);
			return true;
		}
		else if (whichKey == 'O')
		{
			this->selectDot(this->robotDots[this->selectIndex + 1]);
			return true;
		}
		else if (whichKey == 'X')
		{
			if (shiftDown)
			{
				this->selectedDot->segment->changeXMin(.2f);
				this->selectedDot->segment->changeXMax(-.2f);
				this->selectedDot->shape->updateLimitsText();
			}
			else
			{
				this->selectedDot->segment->SetXAvailable(!this->selectedDot->segment->xAvailable);
				this->selectedDot->shape->xLabel->visible = this->selectedDot->segment->xAvailable;
			}
			handled = true;
		}
		else if (whichKey == 'Y')
		{
			this->selectedDot->segment->SetYAvailable(!this->selectedDot->segment->yAvailable);
			this->selectedDot->shape->yLabel->visible = this->selectedDot->segment->yAvailable;
			handled = true;
		}
		else if (whichKey == 'Z')
		{
			this->selectedDot->segment->SetZAvailable(!this->selectedDot->segment->zAvailable);
			this->selectedDot->shape->zLabel->visible = this->selectedDot->segment->zAvailable;
			handled = true;
		}
		if (!handled)
			return false;
		this->selectedDot->shape->SetForegroundTint(
			XMVectorSet(this->selectedDot->segment->xAvailable ? .7f : .2f,
				this->selectedDot->segment->yAvailable ? .7f : .2f,
				this->selectedDot->segment->zAvailable ? .7f : .2f, 1));
		return true;
	}

	void UIMapEditor::deleteLast()
	{
		if (this->robotDots.empty())
			return;

		RobotDot* const r = this->robotDots.back();

		if (this->selectedDot == r)
		{
			this->selectDot(this->robotDots[this->selectIndex - 1]);
		}

		r->shape->Remove();
		this->robotDots.resize(this->robotDots.size() - 1);
		r->segment->parentSkeleton->deleteLastSegment();

		delete r;
	}

	void UIMapEditor::createGlobalSettingsButtons()
	{
		{
			UIButton* const button = new UIButton(this->parentWindow);
			button->SetOnClickCallback([this](UIElement* callingElement, Window* owner, short whichButton, float x, float y) {
				if (this->skeleton != nullptr) {
					this->skeleton->ResetSegments();
				}
				return true;
			});
			button->SetBounds(.1f, .05f);
			button->SetForegroundTint(XMVectorSet(.5f, 1, 1, .8f));
			button->SetPosition(-.4f, .43f);
			UILabel startLabel;
			startLabel.offset = XMVectorSet(0, -.02f, 0, 0);
			startLabel.text = "Reset";
			button->AddTextLabel(startLabel);
			this->AddChild(button);
		}

		{
			UIButton* const button = new UIButton(this->parentWindow);
			button->SetOnUseCallback([this](UIElement* callingElement, int activationMethod, Window* owner) {
				this->SetFocusElement(-1);
				this->setScale(callingElement->getInputNumber<float>());
			});

			button->SetOnClickCallback(clickCallback);
			button->SetBounds(.1f, .05f);
			button->SetForegroundTint(XMVectorSet(.5f, 1, 1, .8f));
			button->SetPosition(-.4f, .33f);
			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.48f, -.02f, 0, 0);
			startLabel.text = "Scale: " + to_string(this->scale);
			button->SetEditableStartIndex(string("Scale: ").size());
			button->AddTextLabel(startLabel);
			button->SetEditable(true);
			this->AddChild(button);
		}
		{
			UIButton* const button = new UIButton(this->parentWindow);
			button->SetOnClickCallback([this](UIElement* callingElement, Window* owner, short whichButton, float x, float y) {
				this->constraintsDiabled = !this->constraintsDiabled;
				return true;
			});
			button->SetBounds(.1f, .05f);
			button->SetForegroundTint(XMVectorSet(.5f, 1, 1, .8f));
			button->SetPosition(-.4f, .23);
			UILabel startLabel;
			startLabel.offset = XMVectorSet(0, -.02f, 0, 0);
			startLabel.text = "Constr";
			button->AddTextLabel(startLabel);
			this->AddChild(button);
		}
		{
			UIButton* const button = new UIButton(this->parentWindow);
			button->SetOnUseCallback([this](UIElement* callingElement, int activationMethod, Window* owner) {
				this->SetFocusElement(-1);
				const int val = callingElement->getInputNumber<int>();
				if (val > 0)
					FractalTreeResources::ikIterationCount = val;
			});

			button->SetOnClickCallback(clickCallback);
			button->SetBounds(.1f, .05f);
			button->SetForegroundTint(XMVectorSet(.5f, 1, 1, .8f));
			button->SetPosition(-.4f, -.38f);
			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.48f, -.02f, 0, 0);
			startLabel.text = "Iterations: " + to_string(FractalTreeResources::ikIterationCount);
			button->SetEditableStartIndex(string("Iterations: ").size());
			button->AddTextLabel(startLabel);
			button->SetEditable(true);
			this->AddChild(button);
		}
		{
			UIButton* const button = new UIButton(this->parentWindow);
			button->SetOnUseCallback([this](UIElement* callingElement, int activationMethod, Window* owner) {
				this->SetFocusElement(-1);
				const int val = callingElement->getInputNumber<int>();
				if (val > 0)
					FractalTreeResources::maxFailCount = val;
			});

			button->SetOnClickCallback(clickCallback);
			button->SetBounds(.1f, .05f);
			button->SetForegroundTint(XMVectorSet(.5f, 1, 1, .8f));
			button->SetPosition(-.4f, -.435f);
			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.48f, -.02f, 0, 0);
			startLabel.text = "Fail Frames: " + to_string(FractalTreeResources::maxFailCount);
			button->SetEditableStartIndex(string("Fail Frames: ").size());
			button->AddTextLabel(startLabel);
			button->SetEditable(true);
			this->AddChild(button);
		}
	}

	void UIMapEditor::createSegmentEditorPane()
	{
		const auto getButtonLabel = [this](UIElement* const button) {
			UILabel startLabel;
			startLabel.offset = XMVectorSet(this->textX, this->textY, 0, 0);
			startLabel.text = "";
			button->AddTextLabel(startLabel);
			button->SetEditable(true);
			button->SetBounds(this->xSize, this->ySize);
			button->SetClickable(true);
			button->SetFocusable(true);
			button->SetPosition(this->startX, this->startY + this->yDelta * this->buttonCount--);

			button->SetForegroundTint(XMVectorSet(1, 1, 1, .5f));
			button->SetEditableStartIndex(0);
			button->SetFocusTint(XMVectorSet(1, 1, 1, .5f));
		};
		{
			UIButton* const button = new UIButton(this->parentWindow);
			button->SetOnUseCallback([this](UIElement* callingElement, int activationMethod, Window* owner) {
				this->selectedDot->segment->changeXMin(callingElement->getInputNumber<float>());
				this->SetFocusElement(-1);
				this->selectedDot->shape->updateLimitsText();
			});

			button->SetOnClickCallback(clickCallback);
			getButtonLabel(button);
			this->editXMinButton = button;
		}
		{
			UIButton* const button = new UIButton(this->parentWindow);
			button->SetOnUseCallback([this](UIElement* callingElement, int activationMethod, Window* owner) {
				this->selectedDot->segment->changeXMax(callingElement->getInputNumber<float>());
				this->SetFocusElement(-1);
				this->selectedDot->shape->updateLimitsText();
			});
			button->SetOnClickCallback(clickCallback);
			getButtonLabel(button);
			this->editXMaxButton = button;
		}

		{
			UIButton* const button = new UIButton(this->parentWindow);
			button->SetOnUseCallback([this](UIElement* callingElement, int activationMethod, Window* owner) {
				this->selectedDot->segment->changeYMin(callingElement->getInputNumber<float>());
				this->SetFocusElement(-1);
				this->selectedDot->shape->updateLimitsText();
			});

			button->SetOnClickCallback(clickCallback);
			getButtonLabel(button);
			this->editYMinButton = button;
		}
		{
			UIButton* const button = new UIButton(this->parentWindow);
			button->SetOnUseCallback([this](UIElement* callingElement, int activationMethod, Window* owner) {
				this->selectedDot->segment->changeYMax(callingElement->getInputNumber<float>());
				this->SetFocusElement(-1);
				this->selectedDot->shape->updateLimitsText();
			});
			button->SetOnClickCallback(clickCallback);
			getButtonLabel(button);
			this->editYMaxButton = button;
		}

		{
			UIButton* const button = new UIButton(this->parentWindow);
			button->SetOnUseCallback([this](UIElement* callingElement, int activationMethod, Window* owner) {
				this->selectedDot->segment->changeZMin(callingElement->getInputNumber<float>());
				this->SetFocusElement(-1);
				this->selectedDot->shape->updateLimitsText();
			});
			button->SetOnClickCallback(clickCallback);
			getButtonLabel(button);
			this->editZMinButton = button;
		}

		{
			UIButton* const button = new UIButton(this->parentWindow);
			button->SetOnUseCallback([this](UIElement* callingElement, int activationMethod, Window* owner) {
				this->selectedDot->segment->changeZMax(callingElement->getInputNumber<float>());
				this->SetFocusElement(-1);
				this->selectedDot->shape->updateLimitsText();
			});

			button->SetOnClickCallback(clickCallback);
			getButtonLabel(button);
			this->editZMaxButton = button;
		}
		{
			UIButton* const button = new UIButton(this->parentWindow);
			button->SetOnUseCallback([this](UIElement* callingElement, int activationMethod, Window* owner) {
				this->selectedDot->segment->changeZDefault(callingElement->getInputNumber<float>());
				this->SetFocusElement(-1);
				this->selectedDot->segment->parentSkeleton->resetSegments();
				this->selectedDot->shape->updateLimitsText();
				this->resetDots();
			});

			button->SetOnClickCallback(clickCallback);
			getButtonLabel(button);
			this->editZRotButton = button;
		}
		{
			UIButton* const button = new UIButton(this->parentWindow);

			button->SetOnClickCallback([this, button](UIElement* callingElement, Window* owner, short whichButton, float x, float y) {
				this->selectedDot->segment->toggleGoal(GoalType::TARGET_DISTANCE);
				this->selectedDot->segment->toggleGoal(GoalType::TARGET_FACING);
				//this->selectedDot->segment->toggleGoal();
				button->GetTextLabels()[0].text = "Is goal: " + to_string(this->selectedDot->segment->hasDistanceGoal());
				return true;
			});
			button->SetBounds(this->xSize, this->ySize);
			button->SetForegroundTint(XMVectorSet(.5f, 1, 1, .3f));
			button->SetPosition(this->startX, this->startY + this->yDelta * this->buttonCount--);
			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.48f, -.5f, 0, 0);
			startLabel.text = "Is goal: ";
			button->AddTextLabel(startLabel);
			button->SetFocusable(false);
			this->isGoalButton = button;
		}
		{
			UIButton* const button = new UIButton(this->parentWindow);

			button->SetOnClickCallback([this, button](UIElement* callingElement, Window* owner, short whichButton, float x, float y) {
				this->selectedDot->segment->toggleGoal(GoalType::SEGMENT_HOLD_POSITION);
				button->GetTextLabels()[0].text = "Is foot: " + to_string(this->selectedDot->segment->hasHoldPositionGoal());
				return true;
			});
			button->SetBounds(this->xSize, this->ySize);
			button->SetForegroundTint(XMVectorSet(.5f, 1, 1, .3f));
			button->SetPosition(this->startX, this->startY + this->yDelta * this->buttonCount--);
			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.48f, -.5f, 0, 0);
			startLabel.text = "Is foot: ";
			button->AddTextLabel(startLabel);
			button->SetFocusable(false);
			this->isFootButton = button;
		}

		{
			UIButton* const button = new UIButton(this->parentWindow);

			button->SetOnClickCallback([this, button](UIElement* callingElement, Window* owner, short whichButton, float x, float y) {
				this->selectedDot->segment->toggleGoal(GoalType::SEGMENT_ABSOLUTE_FACING);
				this->selectedDot->segment->SetAbsoluteFacingGoalVector(XMVectorSet(0, 1, 0, 0));
				button->GetTextLabels()[0].text = "Is vert ang: " + to_string(this->selectedDot->segment->hasAbsoluteFacingGoal());
				return true;
			});
			button->SetBounds(this->xSize, this->ySize);
			button->SetForegroundTint(XMVectorSet(.5f, 1, 1, .3f));
			button->SetPosition(this->startX, this->startY + this->yDelta * this->buttonCount--);
			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.48f, -.5f, 0, 0);
			startLabel.text = "Is vert ang: ";
			button->AddTextLabel(startLabel);
			button->SetFocusable(false);
			this->isAngleGoalButton = button;
		}

		this->AddChild(this->editXMinButton);
		this->AddChild(this->editXMaxButton);
		this->AddChild(this->editYMinButton);
		this->AddChild(this->editYMaxButton);
		this->AddChild(this->editZMinButton);
		this->AddChild(this->editZMaxButton);
		this->AddChild(this->editZRotButton);
		this->AddChild(this->isGoalButton);
		this->AddChild(this->isFootButton);
		this->AddChild(this->isAngleGoalButton);
	}

	UIMapEditor::~UIMapEditor()
	{
		for (const RobotDot* const d : this->robotDots)
		{
			delete d;
		}
	}

	UIPane::UIPane(Window* parentWindow) :
		UIElement(parentWindow, nullptr, false, false, true)
	{

	}

	UIShape::UIShape(Window* parentWindow, const XMVECTOR& pos, RobotDot* dot, float rot) :
		UIElement(parentWindow, nullptr, false, true, true),
		dot(dot)
	{
		int labelInd = 0;
		this->rot = rot;
		this->SetPosition(pos);
		this->makeSmall();
		this->SetForegroundTint(XMVectorSet(1, 1, 1, 1));
		this->textLabels.reserve(10);
		UILabel newXLabel;
		newXLabel.visible = false;
		newXLabel.text = "X (min,max: (,)";// +to_string(;
		newXLabel.offset = XMVectorSet(.55f, 0, 0, 1);
		UILabel newYLabel;
		newYLabel.visible = false;
		newYLabel.offset = XMVectorSet(.55f, -.4, 0, 1);
		newYLabel.text = "Y (min,max: (,)";// +to_string(;
		UILabel newZLabel;
		newZLabel.visible = false;
		newZLabel.offset = XMVectorSet(.55f, -.8, 0, 1);
		newZLabel.text = "Z min,max: (,)";// +to_string(;
		this->AddTextLabel(newXLabel);
		this->AddTextLabel(newYLabel);
		this->AddTextLabel(newZLabel);
		this->xLabel = &this->textLabels[labelInd++];
		this->yLabel = &this->textLabels[labelInd++];
		this->zLabel = &this->textLabels[labelInd++];

		UILabel defaultZLabel;
		defaultZLabel.visible = true;
		defaultZLabel.text = "Rot: ";
		defaultZLabel.offset = XMVectorSet(.55f, -1.2f, 0, 1);
		this->AddTextLabel(defaultZLabel);
		this->defaultZLabel = &this->textLabels[labelInd++];

		this->updateLimitsText();
	}

	void UIShape::makeBig()
	{
		this->SetBounds(XMVectorSet(.08f, .08f, 0, 1));
	}

	void UIShape::makeSmall()
	{
		this->SetBounds(XMVectorSet(.04f, .04f, 0, 1));
	}
	void UIShape::updateLimitsText()
	{
		this->xLabel->text = "X min,max: (" + to_string(static_cast<int>(this->dot->segment->minValues.x * 180 / XM_PI)) + ", " + to_string(static_cast<int>(this->dot->segment->maxValues.x * 180 / XM_PI)) + ")";
		this->yLabel->text = "Y min,max: (" + to_string(static_cast<int>(this->dot->segment->minValues.y * 180 / XM_PI)) + ", " + to_string(static_cast<int>(this->dot->segment->maxValues.y * 180 / XM_PI)) + ")";
		this->zLabel->text = "Z min,max: (" + to_string(static_cast<int>(this->dot->segment->minValues.z * 180 / XM_PI)) + ", " + to_string(static_cast<int>(this->dot->segment->maxValues.z * 180 / XM_PI)) + ")";
		
		this->defaultZLabel->text = "Rot: (" + getVectorStringDeg(this->dot->segment->defaultPitchYawRoll) + ")";
	}
	float RobotDot::getTotalZRot()
	{
		XMFLOAT4 angVec;
		XMStoreFloat4(&angVec, this->segment->defaultPitchYawRoll);
		if (this->parent == nullptr)
			return angVec.z;
		return this->parent->getTotalZRot() + angVec.z;
	}
	UILine::UILine(Window* parentWindow, const XMVECTOR& start, const XMVECTOR& end) :
		UIElement(parentWindow, nullptr, false, false, true)
	{
		const XMVECTOR pos = XMVectorScale(XMVectorAdd(start, end), .5f);
		this->SetPosition(pos);
		XMFLOAT4 ang, start2, end2, len;
		XMStoreFloat4(&ang, XMVector2AngleBetweenVectors(start, end));
		XMStoreFloat4(&len, XMVector2Length(XMVectorSubtract(end, start)));
		XMStoreFloat4(&start2, start);
		XMStoreFloat4(&end2, end);
		this->SetBounds(.005f, len.x);
		const float ang2 = fabs(end2.y - start2.y) > .00001 ? atan2(end2.x - start2.x, end2.y - start2.y) : 0;
		this->rot = -ang2;
		this->SetForegroundTint(XMVectorSet(.1f, 1, .1f, .6f));
	}
}