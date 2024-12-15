#pragma once
#include <optional>

#include "IDataAccess.h"
#include "sqlite3.h"


class DatabaseAccess : public IDataAccess
{
public:
	DatabaseAccess();
	~DatabaseAccess() override;

	// album related functions //
	const std::list<Album> getAlbums() override;
	const std::list<Album> getAlbumsOfUser(const User& user) override;
	void createAlbum(const Album& album) override;
	void deleteAlbum(const std::string& albumName, int userId) override;
	bool doesAlbumExists(const std::string& albumName, int userId) override;
	Album openAlbum(const std::string& albumName) override;
	void closeAlbum(Album& album) override;
	void printAlbums() override;

	// picture related functions //
	void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) override;
	void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) override;
	void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	int getLastPictureId() override;

	// user related functions //
	void printUsers() override;
	User getUser(int userId) override;
	void createUser(User& user) override;
	void deleteUser(const User& user) override;
	bool doesUserExists(int userId) override;
	int getLastUserId() override;

	// user statistics functions //
	int countAlbumsOwnedOfUser(const User& user) override;
	int countAlbumsTaggedOfUser(const User& user) override;
	int countTagsOfUser(const User& user) override;
	float averageTagsPerAlbumOfUser(const User& user) override;

	// tags related statistics functions //
	User getTopTaggedUser() override;
	Picture getTopTaggedPicture() override;
	std::list<Picture> getTaggedPicturesOfUser(const User& user) override;

	// db access related functions //
	bool open() override;
	void close() override;
	void clear() override;


	// helper functions //
	bool doesPictureExistsInAlbum(const std::string& albumName, const std::string& pictureName) const;
	bool doesAlbumExists(const std::string& albumName) const;
	bool doesPictureExists(const std::string& pictureName, int pic_id) const;
	std::list<User> getUsers() const;
	int getAlbumID(const std::string& albumName) const;
	Album getAlbum(const std::string& albumName, std::optional<int> userId = std::nullopt);
	std::list<Picture> getAlbumPictures(const Album& album);
	int getPictureID(const std::string& albumName, const std::string& pictureName) const;
	std::set<int> getPictureTags(const std::string& albumName, const Picture& picture) const;
	std::set<int> getPictureTags(const Picture& picture) const;
	bool doesUserTaggedPicture(const int user_id, const int& pic_id) const;

private:
	sqlite3* db = nullptr; // pointer to the database

	// Wrapper functions for sqlite3_exec //
	void runSQL(const std::string& sql_statement) const;
	void runSQL(const std::string& sql_statement, void* data, int(*callback)(void*, int, char**, char**)) const;

};
