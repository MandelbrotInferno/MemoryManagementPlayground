#pragma once


#include <cstdint>


namespace PersonalMemory
{

	class MemoryAlloc;

	class MemoryPool final
	{
	private:

		friend class MemoryAlloc;

		typedef std::uint32_t u32;

	public:

		MemoryPool(const u32 totalBytes, const u32 blockSize);

		
	private:

		[[nodiscard]] unsigned char* AllocateBlock();

		void FreeBlock(const u32 blockIndex);

		void FreeAll();

	private:

		unsigned char* _data{};
		u32* _dataIndicesTable{};
		u32 _sizeOfIndicesTable{};
		u32 _blockSize{};
		u32 _bytesAllocated{};
		u32 _headIndex{};
		u32 _freeBlocksLeft{};
	};
}