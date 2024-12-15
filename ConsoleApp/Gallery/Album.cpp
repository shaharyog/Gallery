#include "Album.h"

#include <algorithm>

#include "ItemNotFoundException.h"
#include <iomanip>
#include <sstream>


Album::Album(int ownerId, std::string name) :
	m_ownerId(ownerId), m_name(std::move(name)), m_pictures{}
{
	setCreationDateNow();
}

Album::Album(int ownerId, std::string name, std::string creationTime) :
	m_ownerId(ownerId), m_name(std::move(name)), m_creationDate(std::move(creationTime)), m_pictures{}
{
	// Left empty
}


const std::string& Album::getName() const
{
	return m_name;
}

void Album::setName(const std::string& name)
{
	m_name = name;
}

int Album::getOwnerId() const
{
	return m_ownerId;
}

void Album::setOwner(int userId)
{
	m_ownerId = userId;
}

std::string Album::getCreationDate() const
{
	return m_creationDate;
}

void Album::setCreationDate(const std::string& creationTime)
{
	m_creationDate = creationTime;
}

void Album::setCreationDateNow()
{
	const time_t now = time(nullptr);
	std::stringstream oss;
	oss << std::put_time(localtime(&now), "%d/%m/%Y %H:%M:%S");
	m_creationDate = oss.str();
}


Picture Album::getPicture(const std::string& pictureName) const
{
	for (auto& picture : m_pictures) {
		if (pictureName == picture.getName())
			return picture;
	}
	throw ItemNotFoundException("Picture", pictureName);
}


std::list<Picture> Album::getPictures() const
{
	return m_pictures;
}

void Album::untagUserInAlbum(int userId)
{
	for (auto& picture : m_pictures)
		picture.untagUser(userId);
}

void Album::tagUserInAlbum(int userId)
{
	for (auto& picture : m_pictures)
		picture.tagUser(userId);
}

void Album::untagUserInPicture(int userId, const std::string& pictureName)
{
	for (auto& picture : m_pictures) {
		if (picture.getName() == pictureName)
			picture.untagUser(userId);
	}
}

void Album::tagUserInPicture(int userId, const std::string& pictureName)
{
	for (auto& picture : m_pictures) {
		if (picture.getName() == pictureName)
			picture.tagUser(userId);
	}
}

void Album::addPicture(const Picture& picture)
{
	m_pictures.push_back(picture);
}


void Album::removePicture(const std::string& pictureName)
{
	for (const auto& picture : m_pictures) {
		if (pictureName == picture.getName()) {
			m_pictures.remove(picture);
			return;
		}
	}

	throw ItemNotFoundException("Picture", pictureName);
}


bool Album::doesPictureExists(const std::string& name) const
{
	auto isSameName = [&](const Picture& picture) { return name == picture.getName(); };

	return std::any_of(m_pictures.begin(), m_pictures.end(), isSameName);
}

bool Album::operator==(const Album& other) const
{
	return m_ownerId == other.getOwnerId();
}



std::ostream& operator<<(std::ostream& strOut, const Album& album)
{
	strOut << "[" << album.m_name << "] - created by user@"
		<< album.getOwnerId() << " on (" << album.getCreationDate() << ")" << '\n';

	return strOut;
}