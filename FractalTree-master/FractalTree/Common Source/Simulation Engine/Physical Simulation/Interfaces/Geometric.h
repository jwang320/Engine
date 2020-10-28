#pragma once

namespace SimulationEngine
{
	//Interface denoting the existence of a Mesh object
	class Geometric
	{
	protected:
		DirectX::XMVECTOR scale;
		float radius = 1;
	public:
		virtual ~Geometric();

	protected:
		Geometric();

	public:
		void SetScale(const DirectX::XMVECTOR& scale) {this->scale = scale;}
		DirectX::XMVECTOR GetScale() const {return this->scale;}
		float GetRadius() const { return this->radius; }
		DirectX::XMMATRIX GetTransformation() const {return DirectX::XMMatrixScalingFromVector(this->scale);}
	};
}