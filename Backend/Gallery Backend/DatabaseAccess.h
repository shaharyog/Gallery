#pragma once

#include <list>
#include <optional>
#include <sqlite3.h>
#include "Album.h"


class DatabaseAccess
{
public:
	DatabaseAccess();
	~DatabaseAccess();

	// album related functions //
	std::list<Album> getAlbums() const;
	std::list<Album> getAlbumsOfUser(const User& user) const;
	void createAlbum(const Album& album) const;
	void deleteAlbum(const std::string& albumName, int userId) const;
	bool doesAlbumExists(const std::string& albumName, int userId) const;
	Album openAlbum(const std::string& albumName) const;
	void printAlbums() const;

	// picture related functions //
	void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) const;
	void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) const;
	void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) const;
	void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) const;
	int getLastPictureId() const;

	// user related functions //
	void printUsers() const;
	User getUser(int userId) const;
	void createUser(const User& user) const;
	void deleteUser(const User& user) const;
	bool doesUserExists(int userId) const;
	int getLastUserId() const;

	// user statistics functions //
	int countAlbumsOwnedOfUser(const User& user) const;
	int countAlbumsTaggedOfUser(const User& user) const;
	int countTagsOfUser(const User& user) const;
	float averageTagsPerAlbumOfUser(const User& user) const;

	// tags related statistics functions //
	User getTopTaggedUser() const;
	Picture getTopTaggedPicture() const;
	std::list<Picture> getTaggedPicturesOfUser(const User& user) const;

	// db access related functions //
	bool open();
	void close();
	void clear() const;


	// helper functions //
	bool doesPictureExistsInAlbum(const std::string& albumName, const std::string& pictureName) const;
	bool doesAlbumExists(const std::string& albumName) const;
	bool doesPictureExists(const std::string& pictureName, int pic_id) const;
	std::list<User> getUsers() const;
	int getAlbumID(const std::string& albumName) const;
	Album getAlbum(const std::string& albumName, std::optional<int> userId = std::nullopt) const;
	std::list<Picture> getAlbumPictures(const Album& album) const;
	int getPictureID(const std::string& albumName, const std::string& pictureName) const;
	std::set<User> getPictureTags(const std::string& albumName, const Picture& picture) const;
	std::set<User> getPictureTags(const Picture& picture) const;
	bool doesUserTaggedPicture(const int user_id, const int& pic_id) const;

private:
	sqlite3* db = nullptr; // pointer to the database

	// Wrapper functions for sqlite3_exec //
	void runSQL(const std::string& sql_statement) const;
	void runSQL(const std::string& sql_statement, void* data, int(*callback)(void*, int, char**, char**)) const;

};
