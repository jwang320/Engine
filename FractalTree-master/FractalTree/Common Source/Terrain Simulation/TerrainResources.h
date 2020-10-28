#pragma once
#include <unordered_map>
#include <mutex>

#define VOXEL_KEY_BITS 0x1FFFFF
#define VOXEL_SIZE 16

namespace WorldGrowthSimulation
{
	class Voxelizable;

	//an object to represent availability of volume space and resource
	//in the game world.
	//currently, all 3d resources are the same size
	class TerrainResources
	{
	public:
		struct Voxel
		{
			const Voxelizable* const occupier;
			const unsigned short objectType;
			const int x;
			const int y;
			const int z;
		};

		struct MiniVoxel
		{
			const Voxelizable* const occupier;
			const int y;
		};

	private:
		//x y z: element
		std::unordered_map<unsigned long long, Voxel> voxels;
		//map of x,z coordinate to list of voxels at that coordinate in the y-column
		std::unordered_map<unsigned long long, std::vector<MiniVoxel> > columnLists;
		mutable std::mutex modificationMutex; //it is assumed only one thread is a writer.

	public:
		TerrainResources();

		//returns newly created voxel.
		Voxel AddVoxel(const float x, const float y, const float z, const Voxelizable* const occupier, const unsigned short objectType);
		//returns nullptr if no voxel present at location
		const Voxel* GetVoxel(const float x, const float y, const float z) const;
		void RemoveOwned(const void* const owner);

	private:
		int addVoxelToColumn(const int xKey, const int zKey, const MiniVoxel& miniVoxel);
		//return list of voxels in a particular column centered at xKey, zKey
		std::vector<MiniVoxel>* GetVoxelColumn(const int xKey, const int zKey);

	public:
		static inline unsigned long long GetKey(const float x, const float y, const float z)
		{
			const int xKey = (x + (VOXEL_SIZE / 2)) / VOXEL_SIZE;
			const int yKey = (y + (VOXEL_SIZE / 2)) / VOXEL_SIZE;
			const int zKey = (z + (VOXEL_SIZE / 2)) / VOXEL_SIZE;

			return ((xKey & VOXEL_KEY_BITS) |
				(static_cast<unsigned long long>(yKey & VOXEL_KEY_BITS) << 21)) |
				(static_cast<unsigned long long>(zKey & VOXEL_KEY_BITS) << 42);
		}

		static inline unsigned long long GetKey(const int xKey, const int yKey, const int zKey)
		{
			return ((xKey & VOXEL_KEY_BITS) |
				(static_cast<unsigned long long>(yKey & VOXEL_KEY_BITS) << 21)) |
				(static_cast<unsigned long long>(zKey & VOXEL_KEY_BITS) << 42);
		}

		//return map key to list of voxels in a particular column centered at xKey,zKey
		static inline unsigned long long GetColumnKey(const int xKey, const int zKey)
		{
			return ((xKey & VOXEL_KEY_BITS) |
				(static_cast<unsigned long long>(zKey & VOXEL_KEY_BITS) << 21));
		}

		std::vector<std::pair<unsigned long long, DirectX::XMVECTOR> > GetKeyList();
	};
}