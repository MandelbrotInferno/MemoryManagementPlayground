



#include "MemoryPool.hpp"
#include "Custom_bad_alloc.hpp"

#include <Windows.h>
#include <format>
#include <string>

namespace PersonalMemory
{
	MemoryPool::MemoryPool(const u32 totalBytes, const u32 blockSize)
	{
		if (0U == totalBytes || 0U == blockSize) {
			throw Custom_bad_alloc("Requested total byte size or block size of memory pool was zero.\n");
		}

		_data = static_cast<unsigned char*>(VirtualAlloc(NULL, totalBytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
		if (nullptr == _data) {
			auto errorString = std::format("VirtualAlloc() failed in file {}, line number {}.\nCause of failure: {}\n", __FILE__, __LINE__, GetLastError());
			throw Custom_bad_alloc(std::move(errorString));
		}


		_bytesAllocated = (totalBytes + 4096U - 1U) & ~(4096U - 1U);
		_blockSize = (blockSize + 16U - 1U) & ~(16U - 1U);
		_freeBlocksLeft = _bytesAllocated / _blockSize;
		_sizeOfIndicesTable = _freeBlocksLeft;

		_dataIndicesTable = static_cast<u32*>(VirtualAlloc(NULL, _sizeOfIndicesTable * sizeof(u32), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
		if (nullptr == _dataIndicesTable) {
			auto errorString = std::format("VirtualAlloc() failed in file {}, line number {}.\nCause of failure: {}\n", __FILE__, __LINE__, GetLastError());
			throw Custom_bad_alloc(std::move(errorString));
		}

		_headIndex = 0U;


		for (u32 i = 0U; i < _freeBlocksLeft; ++i) {
			_dataIndicesTable[i] = i + 1;
		}
		_dataIndicesTable[_freeBlocksLeft - 1U] = 0xFFFFFFFF;

	}


	void MemoryPool::FreeAll()
	{
		VirtualFree(_data, 0, MEM_RELEASE);
		VirtualFree(_dataIndicesTable, 0, MEM_RELEASE);
	}


	unsigned char* MemoryPool::AllocateBlock()
	{
		if (0U == _freeBlocksLeft) {
			return nullptr;
		}

		const u32 currentFreeBlockIndex = _headIndex;
		_headIndex = _dataIndicesTable[_headIndex];
		_freeBlocksLeft;

		return &_data[currentFreeBlockIndex * _blockSize];
	}


	void MemoryPool::FreeBlock(const u32 blockIndex)
	{
		if (_sizeOfIndicesTable <= blockIndex) {
			return;
		}

		_dataIndicesTable[blockIndex] = _headIndex;
		_headIndex = blockIndex;
	}

}