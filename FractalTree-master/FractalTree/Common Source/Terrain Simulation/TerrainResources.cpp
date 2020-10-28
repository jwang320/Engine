#include "pch.h"

#include "TerrainResources.h"
#include "Voxelizable.h"
#include "..\LSystem\SpatialLSystemSegment.h"

using namespace std;
using namespace DirectX;

namespace WorldGrowthSimulation
{
	TerrainResources::TerrainResources()
	{
		for (int a = 0; a < 10; ++a)
		{
			for (int b = 0; b < 10; ++b)
			{
				//this->AddVoxel(169000 + a * VOXEL_SIZE, 10 * VOXEL_SIZE, 37000 + b * VOXEL_SIZE, nullptr, 0);
			}
		}
	}

	TerrainResources::Voxel TerrainResources::AddVoxel(const float x, const float y, const float z, const Voxelizable* const occupier, const unsigned short objectType)
	{
		const int xKey = (x + (VOXEL_SIZE / 2)) / VOXEL_SIZE;
		const int yKey = (y + (VOXEL_SIZE / 2)) / VOXEL_SIZE;
		const int zKey = (z + (VOXEL_SIZE / 2)) / VOXEL_SIZE;

		const unsigned long long key = TerrainResources::GetKey(xKey, yKey, zKey);
		this->modificationMutex.lock();
		const auto n = this->voxels.insert({ key, { occupier, objectType, xKey, yKey, zKey } }).first->second;
		this->modificationMutex.unlock();
		const int shadeCount = this->addVoxelToColumn(xKey, zKey, {occupier, yKey});
		if (occupier != nullptr)
		{
			occupier->verticalCoverCount += shadeCount;
			++occupier->totalVoxelCount;
		}
		return n;
	}

	const TerrainResources::Voxel* TerrainResources::GetVoxel(const float x, const float y, const float z) const
	{
		const auto possibleVoxel = this->voxels.find(TerrainResources::GetKey(x, y, z));
		if (possibleVoxel == this->voxels.end())
		{
			return nullptr;
		}

		return &possibleVoxel->second;
	}

	vector<TerrainResources::MiniVoxel>* TerrainResources::GetVoxelColumn(const int xKey, const int zKey)
	{
		auto possibleColumn = this->columnLists.find(GetColumnKey(xKey, zKey));
		if (possibleColumn == this->columnLists.end())
		{
			return nullptr;
		}

		return &possibleColumn->second;
	}

	void TerrainResources::RemoveOwned(const void* const owner)
	{
		vector<long long> removeList;
		for (pair<const long long, Voxel> p : this->voxels)
		{
			if (p.second.objectType == 1)
			{
				if (static_cast<const SpatialLSystemSegment*>(p.second.occupier)->GetParentSystem() == owner)
				{
					removeList.push_back(p.first);
				}
			}
		}

		if (!removeList.empty())
		{
			this->modificationMutex.lock();
			for (const long long key : removeList)
			{
				this->voxels.erase(key);
			}
			this->modificationMutex.unlock();
		}
	}

	int TerrainResources::addVoxelToColumn(const int xKey, const int zKey, const MiniVoxel& miniVoxel)
	{
		vector<MiniVoxel>* voxelColumnList = this->GetVoxelColumn(xKey, zKey);
		if (voxelColumnList == nullptr)
		{
			voxelColumnList = &this->columnLists.insert({ GetColumnKey(xKey, zKey), vector<MiniVoxel>() }).first->second;
		}

		voxelColumnList->push_back(miniVoxel);

		int aboveCount = 0;
		for (const MiniVoxel& voxel : *voxelColumnList)
		{
			if (voxel.y < miniVoxel.y)
			{
				if (voxel.occupier != nullptr)
				{
					++voxel.occupier->verticalCoverCount;
				}
			}
			else if (voxel.y > miniVoxel.y)
			{
				++aboveCount;
			}
		}

		return aboveCount;
	}

	vector<pair<unsigned long long, DirectX::XMVECTOR> > TerrainResources::GetKeyList()
	{
		vector<pair<unsigned long long, DirectX::XMVECTOR> > ret;
		this->modificationMutex.lock(); //locking only necessary for rendering voxel grid
		for (const auto voxel : this->voxels)
		{
			//if (voxel.second.occupier != nullptr)
			//	continue;

			ret.push_back({ voxel.first, XMVectorSet(voxel.second.x, voxel.second.y, voxel.second.z, 1) });
		}
		this->modificationMutex.unlock();
		return ret;
	}
}
