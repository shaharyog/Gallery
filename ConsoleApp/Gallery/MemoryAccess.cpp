#include <map>
#include <algorithm>

#include "ItemNotFoundException.h"
#include "MemoryAccess.h"

#include "Colors.h"
#include "Constants.h"


void MemoryAccess::printAlbums()
{
	if (m_albums.empty()) {
		throw MyException("There are no existing albums.");
	}

	std::cout << GREEN << "Album list:" << '\n' << RESET;
	std::cout << GREEN << "-----------" << '\n' << RESET;

	for (const Album& album : m_albums)
		std::cout << GREEN << std::setw(5) << "* " << album << RESET;
}

bool MemoryAccess::open()
{
	// create some dummy albums
	for (int i = 0; i < 5; ++i) {
		// create some dummy users
		std::stringstream name("User_" + std::to_string(i));

		User user(i, name.str());
		createUser(user);

		m_albums.push_back(createDummyAlbum(user));
	}

	return true;
}

void MemoryAccess::clear()
{
	m_users.clear();
	m_albums.clear();
}

auto MemoryAccess::getAlbumIfExists(const std::string& albumName)
{
	auto result = std::find_if(std::begin(m_albums), std::end(m_albums), [&](auto& album) { return album.getName() == albumName; });

	if (result == std::end(m_albums))
		throw ItemNotFoundException("Album", albumName);

	return result;

}

Album MemoryAccess::createDummyAlbum(const User& user)
{
	std::stringstream name("Album_" + std::to_string(user.getId()));

	Album album(user.getId(), name.str());

	for (int i = 1; i < 3; ++i) {
		std::stringstream picName("Picture_" + std::to_string(i));

		Picture pic(i++, picName.str());
		pic.setPath("C:\\Pictures\\" + picName.str() + ".bmp");

		album.addPicture(pic);
	}

	return album;
}

void MemoryAccess::cleanUserData(const User& user)
{

	// remove all albums associated with the user
	const std::list<Album>& albums = getAlbumsOfUser(user);
	for (const auto& album : albums)
		m_albums.remove(album);

	// remove all tags associated with the user
	for (auto& album : m_albums)
		album.untagUserInAlbum(user.getId());
}

const std::list<Album> MemoryAccess::getAlbums()
{
	return m_albums;
}

const std::list<Album> MemoryAccess::getAlbumsOfUser(const User& user)
{
	std::list<Album> albumsOfUser;
	for (const auto& album : m_albums) {
		if (album.getOwnerId() == user.getId())
			albumsOfUser.push_back(album);
	}
	return albumsOfUser;
}

void MemoryAccess::createAlbum(const Album& album)
{
	m_albums.push_back(album);
}

void MemoryAccess::deleteAlbum(const std::string& albumName, int userId)
{
	for (auto& album : m_albums)
	{
		if (album.getName() == albumName && album.getOwnerId() == userId)
		{
			m_albums.remove(album);
			return;
		}
	}
}

bool MemoryAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	auto isSameAlbum = [&](const Album& album) { return album.getName() == albumName && album.getOwnerId() == userId; };
	return std::any_of(m_albums.cend(), m_albums.cend(), isSameAlbum);
}

Album MemoryAccess::openAlbum(const std::string& albumName)
{
	for (auto& album : m_albums) {
		if (albumName == album.getName())
			return album;
	}
	throw MyException("No album with name " + albumName + " exists");
}

void MemoryAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
	const auto result = getAlbumIfExists(albumName);
	result->addPicture(picture);
}

void MemoryAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
	const auto result = getAlbumIfExists(albumName);
	result->removePicture(pictureName);
}

void MemoryAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	const auto result = getAlbumIfExists(albumName);
	result->tagUserInPicture(userId, pictureName);
}

void MemoryAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	const auto result = getAlbumIfExists(albumName);
	result->untagUserInPicture(userId, pictureName);
}

int MemoryAccess::getLastPictureId()
{
	return FIRST_PICTURE_ID;
}

void MemoryAccess::closeAlbum(Album&)
{
	// basically here we would like to delete the allocated memory we got from openAlbum
}

// ******************* User ******************* 
void MemoryAccess::printUsers()
{
	std::cout << GREEN << "Users list:" << '\n' << RESET;
	std::cout << GREEN << "-----------" << '\n' << RESET;

	for (const auto& user : m_users)
		std::cout << GREEN << user << '\n' << RESET;
}

User MemoryAccess::getUser(int userId) {
	for (const auto& user : m_users) {
		if (user.getId() == userId)
			return user;
	}

	throw ItemNotFoundException("User", userId);
}

int MemoryAccess::getLastUserId()
{
	return FIRST_USER_ID;
}

void MemoryAccess::createUser(User& user)
{
	m_users.push_back(user);
}

void MemoryAccess::deleteUser(const User& user)
{
	for (auto& user_to_delete : m_users)
	{
		if (user_to_delete == user)
		{
			cleanUserData(user_to_delete);
			m_users.remove(user_to_delete);
			return;
		}
	}
}

bool MemoryAccess::doesUserExists(int userId)
{
	auto isSameUser = [&](const User& user) { return user.getId() == userId; };

	return std::any_of(m_users.cbegin(), m_users.cend(), isSameUser);
}


// user statistics
int MemoryAccess::countAlbumsOwnedOfUser(const User& user)
{
	auto isUserOwner = [&](const Album& album) { return album.getOwnerId() == user.getId(); };
	return std::count_if(m_albums.cbegin(), m_albums.cend(), isUserOwner);
}

int MemoryAccess::countAlbumsTaggedOfUser(const User& user)
{
	int albumsCount = 0;

	for (const auto& album : m_albums) {
		const std::list<Picture>& pics = album.getPictures();

		for (const auto& picture : pics) {
			if (picture.isUserTagged(user)) {
				albumsCount++;
				break;
			}
		}
	}

	return albumsCount;
}

int MemoryAccess::countTagsOfUser(const User& user)
{
	int tagsCount = 0;

	for (const auto& album : m_albums) {
		const std::list<Picture>& pics = album.getPictures();

		for (const auto& picture : pics) {
			if (picture.isUserTagged(user))
				tagsCount++;
		}
	}

	return tagsCount;
}

float MemoryAccess::averageTagsPerAlbumOfUser(const User& user)
{
	const int albumsTaggedCount = countAlbumsTaggedOfUser(user);

	if (albumsTaggedCount == 0)
		return 0;

	return static_cast<float>(countTagsOfUser(user)) / static_cast<float>(albumsTaggedCount);
}

User MemoryAccess::getTopTaggedUser()
{
	std::map<int, int> userTagsCountMap;

	for (const auto& album : m_albums) {
		for (const auto& picture : album.getPictures()) {

			const std::set<int>& userTags = picture.getUserTags();
			for (const auto& user : userTags) {
				//As map creates default constructed values, 
				//users which we haven't yet encountered will start from 0
				userTagsCountMap[user]++;
			}
		}
	}

	if (userTagsCountMap.empty()) {
		throw MyException("There isn't any tagged user.");
	}

	int topTaggedUser = -1;
	int currentMax = -1;
	for (const auto entry : userTagsCountMap) {
		if (entry.second < currentMax)
			continue;

		topTaggedUser = entry.first;
		currentMax = entry.second;
	}

	if (-1 == topTaggedUser)
		throw MyException("Failed to find most tagged user");

	return getUser(topTaggedUser);
}

Picture MemoryAccess::getTopTaggedPicture()
{
	int currentMax = -1;
	const Picture* mostTaggedPic = nullptr;
	for (const auto& album : m_albums) {
		for (const Picture& picture : album.getPictures()) {
			const int tagsCount = picture.getTagsCount();
			if (tagsCount == 0)
				continue;

			if (tagsCount <= currentMax)
				continue;

			mostTaggedPic = &picture;
			currentMax = tagsCount;
		}
	}

	if (nullptr == mostTaggedPic)
		throw MyException("There isn't any tagged picture.");

	return *mostTaggedPic;
}

std::list<Picture> MemoryAccess::getTaggedPicturesOfUser(const User& user)
{
	std::list<Picture> pictures;

	for (const auto& album : m_albums) {
		for (const auto& picture : album.getPictures()) {
			if (picture.isUserTagged(user))
				pictures.push_back(picture);
		}
	}

	return pictures;
}
