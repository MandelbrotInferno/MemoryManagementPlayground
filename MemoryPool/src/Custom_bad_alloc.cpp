



#include "Custom_bad_alloc.hpp"



Custom_bad_alloc::Custom_bad_alloc(std::string&& msg)
	:std::bad_alloc(), _msg(std::move(msg))
{

}


const char* Custom_bad_alloc::what() const noexcept
{
	return _msg.data();
}