#include "Picture.h"

#include <algorithm>
#include <ctime>
#include <sstream>


Picture::Picture(int id, std::string name) :
	m_pictureId(id), m_name(std::move(name))
{
	setCreationDateNow();
}

Picture::Picture(int id, std::string name, std::string pathOnDisk, std::string creationDate)
	: m_pictureId(id), m_name(std::move(name)), m_pathOnDisk(std::move(pathOnDisk)), m_creationDate(
		std::move(creationDate))
{}

int Picture::getId() const
{
	return m_pictureId;
}

void Picture::setId(int id)
{
	m_pictureId = id;
}

const std::string& Picture::getName() const
{
	return m_name;
}

void Picture::setName(const std::string& name)
{
	m_name = name;
}

const std::string& Picture::getPath() const
{
	return m_pathOnDisk;
}

void Picture::setPath(const std::string& location)
{
	m_pathOnDisk = location;
}

const std::string& Picture::getCreationDate() const
{
	return m_creationDate;
}

void Picture::setCreationDate(const std::string& creationTime)
{
	m_creationDate = creationTime;
}

void Picture::setCreationDateNow()
{
	const time_t now = time(nullptr);
	std::tm local_time;
	localtime_s(&local_time, &now);

	std::stringstream oss;
	oss << std::put_time(&local_time, "%Y-%m-%dT%H:%M:%S");
	m_creationDate = oss.str();
}

bool Picture::isUserTagged(const User& userToTag) const
{
	return std::any_of(m_usersTagged.cbegin(), m_usersTagged.cend(), [&](const User& user) { return user.getId() == userToTag.getId(); });
}

bool Picture::isUserTagged(int userId) const
{
	return std::any_of(m_usersTagged.cbegin(), m_usersTagged.cend(), [&](const User& user) { return user.getId() == userId; });
}

void Picture::tagUser(const User& user)
{
	m_usersTagged.insert(user);
}

void Picture::untagUser(const User& userToUntag)
{
	if (isUserTagged(userToUntag))
		m_usersTagged.erase(std::find_if(m_usersTagged.cbegin(), m_usersTagged.cend(), [&](const User& user) { return user.getId() == userToUntag.getId(); }));
}

void Picture::untagUser(int userId)
{
	m_usersTagged.erase(std::find_if(m_usersTagged.cbegin(), m_usersTagged.cend(), [&](const User& user) { return user.getId() == userId; }));
}

int Picture::getTagsCount() const
{
	return static_cast<int>(m_usersTagged.size());
}

const std::set<User>& Picture::getUsersTagged() const
{
	return m_usersTagged;
}

bool Picture::operator==(const Picture& other) const
{
	return m_pictureId == other.getId();
}

std::ostream& operator<<(std::ostream& os, const Picture& pic) {
	os << "Picture@" << pic.m_pictureId << ": ["
		<< pic.m_name << ", " << pic.m_creationDate << ", " << pic.m_pathOnDisk <<
		"] " << pic.getTagsCount() << " users tagged : ";

	for (const auto& user : pic.m_usersTagged) {
		os << "(" << user.getId() << ") ";
	}
	return os;
}