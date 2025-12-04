



#include "MemoryAlloc.hpp"
#include "Custom_bad_alloc.hpp"

#include <iostream>


struct TEST
{
	char d;
	double c;
	float b;
	int a;
};




int main()
{
	try {
		
		INITIALIZE_ALLOCATOR(ConvertGBtoBytes(2));

		auto& memAlloc = GET_ALLOCATOR();

		TEST* test = static_cast<TEST*>(memAlloc.AllocateBlock(50U));

		if (nullptr == test) {
			std::cout << "Failed to allocate" << std::endl;
			return 0;
		}

		test->a = 2;
		test->c = 28.;


		memAlloc.CommitBlock(&test);

		TEST* test2 = (TEST*)memAlloc.RetrieveAllocatedBlock(&test);


		std::cout << test2->a << std::endl;
		std::cout << test2->c << std::endl;

		memAlloc.CommitBlock(&test2);

		memAlloc.FreeBlock(test2);
	}
	catch (const Custom_bad_alloc& exception) {
		std::cout << exception.what() << std::endl;
	}

	return 0;
}