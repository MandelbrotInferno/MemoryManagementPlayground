





#include <Windows.h>
#include <cstdint>
#include <iostream>


typedef std::uint8_t u8;
typedef std::uint32_t u32;


DWORD exceptionCodeGlobal;
ULONG_PTR exceptionInform[15];

static int Filter(const u32 exceptionCode, const EXCEPTION_POINTERS* exceptionPtr)
{

	exceptionCodeGlobal = exceptionPtr->ExceptionRecord->ExceptionCode;
	exceptionInform[0] = exceptionPtr->ExceptionRecord->ExceptionInformation[0];
	exceptionInform[1] = exceptionPtr->ExceptionRecord->ExceptionInformation[1];
	exceptionInform[2] = exceptionPtr->ExceptionRecord->ExceptionInformation[2];


	switch (exceptionCode) {
	case EXCEPTION_ACCESS_VIOLATION:
	{
		return EXCEPTION_EXECUTE_HANDLER;
	}break;

	case EXCEPTION_IN_PAGE_ERROR:
	{
		return EXCEPTION_EXECUTE_HANDLER;
	}break;

	default:
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
	}
}



int main()
{

	u8* data = static_cast<u8*>(VirtualAlloc(NULL, 4096, MEM_RESERVE, PAGE_READWRITE));



	if (nullptr == data) {
		std::cout << "VirtualAlloc() failed:" << std::endl;
		return 0;
	}


	__try {
		data[0] = 0;
	}

	__except(Filter(GetExceptionCode(), GetExceptionInformation()))
	{

		switch (exceptionCodeGlobal) {
		case EXCEPTION_ACCESS_VIOLATION:
		{
			const auto virtualAddress = exceptionInform[1];
			std::cout << "EXCEPTION_ACCESS_VIOLATION: ";
			if (0 == exceptionInform[0]) {
				std::cout << "Tried to read an inaccessible address at ";
			}
			if (1 == exceptionInform[0]) {
				std::cout << "Tried to write to an inaccessible address at ";
			}

			std::cout << std::hex << "0x" << virtualAddress << std::endl;;

		}break;

		default:
		{
		}
		}


	}

	VirtualFree(data, 0, MEM_RELEASE);


	return 0;
}