




#include "MemoryAlloc.hpp"
#include "Custom_bad_alloc.hpp"

#include <format>
#include <Windows.h>
#include <print>

namespace PersonalMemory
{

	MemoryAlloc& MemoryAlloc::GetInstance(const u32 totalBytes, const char* fileName, const u32 lineNumber)
	{
		static MemoryAlloc memoryAlloc(totalBytes, fileName, lineNumber);
		return memoryAlloc;
	}


	MemoryAlloc::MemoryAlloc(const u32 totalBytes, const char* fileName, const u32 lineNumber)
		: _pool256(totalBytes, 256U), _fileNameFirstTimeInit(fileName)
		, _lineNumberFirstTimeInit(lineNumber)
	{
		_reserved4kSafeMemories.resize(totalNumFreeSafePages);

		_baseReserved4kSafeMemories = static_cast<unsigned char*>(VirtualAlloc(NULL, 65536 * totalNumFreeSafePages, MEM_RESERVE, PAGE_NOACCESS));
		if (nullptr == _baseReserved4kSafeMemories) {
			auto errorString = std::format("VirtualAlloc() failed in file {}, line number {}.\nCause of failure: {}\n", __FILE__, __LINE__, GetLastError());
			throw Custom_bad_alloc(std::move(errorString));
		}
		
		for (size_t i = 0U; i < totalNumFreeSafePages; ++i) {
			auto* returnedPtr = VirtualAlloc(_baseReserved4kSafeMemories +  (4096 * ((2U * i) + 1U)), 4096, MEM_COMMIT, PAGE_READWRITE);
			if (nullptr == returnedPtr) {
				auto errorString = std::format("VirtualAlloc() failed in file {}, line number {}.\nCause of failure: {}\n", __FILE__, __LINE__, GetLastError());
				throw Custom_bad_alloc(std::move(errorString));
			}
			_reserved4kSafeMemories[i] = static_cast<unsigned char*>(returnedPtr);
		}

		_magic4kMemory = static_cast<unsigned char*>(VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
		if (nullptr == _magic4kMemory) {
			auto errorString = std::format("VirtualAlloc() failed in file {}, line number {}.\nCause of failure: {}\n", __FILE__, __LINE__, GetLastError());
			throw Custom_bad_alloc(std::move(errorString));
		}

		for (size_t i = 0U; i < 4096; ++i) {
			_magic4kMemory[i] = 0xFF;
		}

	}



	[[nodiscard]] void* MemoryAlloc::AllocateBlock(const u32 blockSize)
	{
		auto pair = std::make_pair<unsigned char*, u64>(nullptr, 0U);

		if (blockSize <= 256U) {
			pair.first = _pool256.AllocateBlock();
			pair.second = (pair.first - _pool256._data) / 256U;

			pair.second |= 0x10000000000; // injecting 256 in high 32 bits to say it belongs to _pool256
		}
		else {
			return nullptr;
		}

		auto* safe4kMem = GetNewSafe4kMemory();

		memcpy(safe4kMem, &pair, 16);
		safe4kMem += 16U;

		return safe4kMem;
	}

	void MemoryAlloc::FreeBlock(void* block)
	{
		u64 blockIndex{};
		memcpy(&blockIndex, &block, 8);

		const u32 poolID = blockIndex >> 32;

		switch (poolID) {
		
		case 256:
		{
			const u32 realBlockIndex = blockIndex & 0x0000000011111111;
			_pool256.FreeBlock(realBlockIndex);

		}break;

		}

	}



	[[nodiscard]] void* MemoryAlloc::RetrieveAllocatedBlock(void* block)
	{
		u64 blockIndex{};
		memcpy(&blockIndex, block, 8);

		const u32 poolID = blockIndex >> 32;
		const u32 realBlockIndex = 0x0000000011111111 & blockIndex;
		auto pair = std::make_pair<unsigned char*, MemoryAlloc::u64>(nullptr, 0U);

		switch (poolID) {

		case 256:
		{
			pair.first = &_pool256._data[realBlockIndex * 256];
			pair.second = blockIndex;
		}break;

		default:
		{
			return nullptr;
		}
		}

		auto* safe4kMem = GetNewSafe4kMemory();
		memcpy(safe4kMem, &pair, 16U);
		safe4kMem += 16U;

		memcpy(safe4kMem, pair.first, poolID);

		return safe4kMem;
	}



	MemoryAlloc::~MemoryAlloc()
	{
		_pool256.FreeAll();
	}


	void MemoryAlloc::CommitBlock(void* localVar)
	{
		unsigned char* safe4kMem{};
		if (nullptr == localVar) {
			throw Custom_bad_alloc("CommitBlock() failed due to input being null");
		}
		memcpy(&safe4kMem, localVar, 8);

		safe4kMem = safe4kMem - 16U;

		auto pair = std::make_pair<unsigned char*, u64>(nullptr, 0U);
		memcpy(&pair, safe4kMem, 16U);

		const u32 poolID = pair.second >> 32;

		safe4kMem += 16U;

		memcpy(pair.first, safe4kMem, poolID);

		memcpy(localVar, &pair.second, 8);

		ReturnSafe4kMemory();

	}

}