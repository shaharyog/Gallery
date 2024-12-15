#pragma once
#include <sstream>
#include "MyException.h"

class ItemNotFoundException : public MyException {
public:
	ItemNotFoundException(const std::string& item, int id) : MyException() 
																  
	{
		m_message = item + " with id '" + std::to_string(id) + "' does not exist";
	}
	ItemNotFoundException(const std::string& item, const std::string& name) : MyException()
	{
		m_message = item + " with name '" + name + "' does not exist";
	}

	const char* what() const noexcept override
	{
		return m_message.c_str();
	}
};

