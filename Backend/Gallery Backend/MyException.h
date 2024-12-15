#pragma once
#include <exception>
#include <string>

class MyException : public std::exception
{
public:
	MyException(std::string message) : m_message(std::move(message)) {}
	MyException() = default;
	const char* what() const noexcept override { return m_message.c_str(); }

protected:
	std::string m_message;
};
