#pragma once

namespace RTSCore
{
	class SpaceGameWorld;
	class RTSUnit;

	class RTSUnitOrder
	{
	private:
		int globalId;
		const unsigned int owningUnitId;
		SpaceGameWorld* const gameWorld;
		RTSUnit* const owningUnit;
		const int orderType;
	protected:

		RTSUnitOrder(SpaceGameWorld* const gameWorld, unsigned int owningUnitId, int orderType);
		RTSUnit* getOwner();

	public:
		virtual bool CheckCompletionCondition() = 0;
		virtual void Initialize() {}
		virtual void Update() {}
		virtual void Complete() {}
		virtual void Cancel() {}

	public:
		void SetGlobalId(int globalId) { this->globalId = globalId; }
		int GetGlobalId() const { return this->globalId; }
		int getOrderType() const {return this->orderType;}
		virtual ~RTSUnitOrder() {}
	};
}