#include "User.h"
#include <iomanip>


User::User(int id, std::string name) :
	m_id(id), m_name(std::move(name))
{}

int User::getId() const
{
	return m_id;
}

void User::setId(int id)
{
	m_id = id;
}

const std::string& User::getName() const
{
	return m_name;
}

void User::setName(const std::string& name)
{
	m_name = name;
}

bool User::operator==(const User& other) const
{
	return m_id == other.getId();
}

bool User::operator==(int id) const {
	return m_id == id;
}

bool User::operator<(const User& other) const
{
	return m_id < other.getId();
}

std::ostream& operator<<(std::ostream& os, const User& user) {
	os << std::setw(5) << "   + @" << user.m_id << " - " << user.m_name;
	return os;
}