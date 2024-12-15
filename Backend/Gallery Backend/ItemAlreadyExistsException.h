#pragma once

#include "MyException.h"

class ItemAlreadyExistsException : public MyException {
public:
	ItemAlreadyExistsException(const std::string& item, int id) : MyException()

	{
		m_message = item + " with id '" + std::to_string(id) + "' already exist";
	}
	ItemAlreadyExistsException(const std::string& item, const std::string& name) : MyException()
	{
		m_message = item + " with name '" + name + "' already exist";
	}

	const char* what() const noexcept override
	{
		return m_message.c_str();
	}
};
