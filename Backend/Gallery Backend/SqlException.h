#pragma once
#include "MyException.h"

class SqlException : public MyException {
public:
	SqlException(const std::string& errMsg) : MyException()
	{
		m_message = "SQL Error occured: " + errMsg;
	}
};
