#pragma once



#include "MemoryPool.hpp"

#include <utility>
#include <vector>
#include <cassert>


namespace PersonalMemory
{

	class MemoryAlloc final
	{
	private:

		typedef std::uint32_t u32;
		typedef std::uint64_t u64;

	public:

		static MemoryAlloc& GetInstance(const u32 totalBytes, const char* fileName = __FILE__
			, const u32 lineNumber = __LINE__);

		[[nodiscard]] void* AllocateBlock(const u32 blockSize);

		void FreeBlock(void* block);

		[[nodiscard]] void* RetrieveAllocatedBlock(void* block);

		void CommitBlock(void* localVar);

		~MemoryAlloc();

	private:

		MemoryAlloc(const u32 totalBytes, const char* fileName = __FILE__, const u32 lineNumber = __LINE__);

		unsigned char* GetNewSafe4kMemory()
		{
			assert(_currentFree4kSafeMemory < totalNumFreeSafePages);
			unsigned char* safe4kMem = _reserved4kSafeMemories[_currentFree4kSafeMemory];
			++_currentFree4kSafeMemory;
			return safe4kMem;
		}

		void ReturnSafe4kMemory()
		{
			--_currentFree4kSafeMemory;
		}

	private:

		static constexpr size_t totalNumFreeSafePages = 512U;
		std::vector<unsigned char*> _reserved4kSafeMemories{};

		MemoryPool _pool256;

		unsigned char* _baseReserved4kSafeMemories{};
		unsigned char* _magic4kMemory{};

		const char* _fileNameFirstTimeInit{};
		const u32 _lineNumberFirstTimeInit{};

		u32 _currentFree4kSafeMemory{};
	};
}

inline std::uint32_t ConvertGBtoBytes(const std::uint32_t gb)
{
	return gb * 1024U * 1024U * 1024U;
}

inline std::uint32_t ConvertMbToBytes(const std::uint32_t mb)
{
	return mb * 1024U * 1024U;
}


#define INITIALIZE_ALLOCATOR(totalNum)\
PersonalMemory::MemoryAlloc::GetInstance(totalNum, __FILE__, __LINE__);


#define GET_ALLOCATOR()\
PersonalMemory::MemoryAlloc::GetInstance(0U, __FILE__, __LINE__);
