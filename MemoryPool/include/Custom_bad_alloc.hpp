#pragma once



#include <new>
#include <string>

class Custom_bad_alloc : public std::bad_alloc
{
public:

	Custom_bad_alloc(std::string&& msg);
	
	virtual const char* what() const noexcept;

private:

	std::string _msg{};

};