#include "Picture.h"
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
	std::stringstream oss;
	oss << std::put_time(localtime(&now), "%d/%m/%Y %H:%M:%S");
	m_creationDate = oss.str();
}

bool Picture::isUserTagged(const User& user) const
{
	return std::find(m_usersTags.begin(), m_usersTags.end(), user.getId()) != m_usersTags.end();
}

bool Picture::isUserTagged(int userId) const
{
	return std::find(m_usersTags.begin(), m_usersTags.end(), userId) != m_usersTags.end();
}

void Picture::tagUser(const User& user)
{
	m_usersTags.insert(user.getId());
}

void Picture::tagUser(int userId)
{
	m_usersTags.insert(userId);
}

void Picture::untagUser(const User& user)
{
	if (isUserTagged(user))
		m_usersTags.erase(std::find(m_usersTags.begin(), m_usersTags.end(), user.getId()));
}

void Picture::untagUser(int userId)
{
	if (isUserTagged(userId)) {
		m_usersTags.erase(std::find(m_usersTags.begin(), m_usersTags.end(), userId));
	}
}

int Picture::getTagsCount() const
{
	return static_cast<int>(m_usersTags.size());
}

const std::set<int>& Picture::getUserTags() const
{
	return m_usersTags;
}

bool Picture::operator==(const Picture& other) const
{
	return m_pictureId == other.getId();
}

std::ostream& operator<<(std::ostream& os, const Picture& pic) {
	os << "Picture@" << pic.m_pictureId << ": ["
		<< pic.m_name << ", " << pic.m_creationDate << ", " << pic.m_pathOnDisk <<
		"] " << pic.getTagsCount() << " users tagged : ";

	for (const auto user : pic.m_usersTags) {
		os << "(" << user << ") ";
	}
	return os;
}