#include "DatabaseAccess.h"
#include <io.h>

#include "CallbackFuncs.h"
#include "Colors.h"
#include "Constants.h"
#include "ItemAlreadyExistsException.h"
#include "ItemNotFoundException.h"
#include "MyException.h"
#include "SqlException.h"


DatabaseAccess::DatabaseAccess()
{
	open();
}

DatabaseAccess::~DatabaseAccess()
{
	close();
}

// album related functions //
std::list<Album> DatabaseAccess::getAlbums() const
{
	const std::string getAllAlbumsSQL = "SELECT * FROM ALBUMS;";
	std::list<Album> albums;
	
	runSQL(getAllAlbumsSQL, &albums, getAlbumsCallback);

	for (auto& album : albums)
		album = openAlbum(album.getName());

	return albums;
}

std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user) const
{
	if (!doesUserExists(user.getId()))
		throw ItemNotFoundException("User ", user.getId());

	const std::string query = "SELECT * FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId()) + ";";
	std::list<Album> albums;

	runSQL(query, &albums, getAlbumsCallback);

	for (auto& album : albums)
		album = openAlbum(album.getName());

	return albums;
}

void DatabaseAccess::createAlbum(const Album& album) const
{
	if (doesAlbumExists(album.getName()))
		throw ItemAlreadyExistsException("Album", album.getName());

	if (!doesUserExists(album.getOwnerId()))
		throw ItemNotFoundException("User", album.getOwnerId());

	const std::string createAlbumSQL =
		"INSERT INTO ALBUMS(NAME, USER_ID, CREATION_DATE) "
		"VALUES ('" + album.getName() + "', '" + std::to_string(album.getOwnerId()) + "', '" + album.getCreationDate() + "');";

	runSQL(createAlbumSQL);
}

void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId) const
{
	if (!doesAlbumExists(albumName, userId))
		throw ItemNotFoundException("Album", albumName);

	const int albumID = getAlbumID(albumName);

	const std::string deleteAlbumTagsSql = "DELETE FROM TAGS WHERE PICTURE_ID IN (SELECT ID FROM PICTURES WHERE ALBUM_ID = " + std::to_string(albumID) + ");";
	runSQL(deleteAlbumTagsSql);

	const std::string deleteAlbumPicturesSql = "DELETE FROM PICTURES WHERE ALBUM_ID = " + std::to_string(albumID) + ";";
	runSQL(deleteAlbumPicturesSql);

	const std::string deleteAlbumSql = "DELETE FROM ALBUMS WHERE NAME = '" + albumName + "' AND USER_ID = " + std::to_string(userId) + ";";
	runSQL(deleteAlbumSql);

	const  std::string vacuumSql = "VACUUM;";
	runSQL(vacuumSql);
}

bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId) const
{
	const std::string sqlStatement = "SELECT 1 FROM ALBUMS WHERE NAME = '" + albumName + "' AND USER_ID = " + std::to_string(userId) + " LIMIT 1;";
	bool doesAlbumExist = false;

	// run the sql and pass the albumExistsCallback as the callback, which will set the doesAlbumExist flag to true.
	runSQL(sqlStatement, &doesAlbumExist, existenceCallback);

	return doesAlbumExist;
}

Album DatabaseAccess::openAlbum(const std::string& albumName) const
{
	Album album;

	try {
		album = getAlbum(albumName);
	}
	catch (const ItemNotFoundException& e)
	{
		std::cerr << e.what() << '\n';
		return { -1, "" };
	}

	const User owner = getUser(album.getOwnerId());
	album.setOwnerName(owner.getName());

	std::list<Picture> pictures = getAlbumPictures(album);

	for (Picture& picture : pictures)
	{
		std::set<User> users_tagged = getPictureTags(album.getName(), picture);

		for (const auto& user : users_tagged)
			picture.tagUser(user);

		album.addPicture(picture);
	}

	return album;

}


void DatabaseAccess::printAlbums() const
{
	const std::list<Album> albums = getAlbums();

	if (albums.empty())
		throw MyException("There are no existing albums.");

	std::cout << GREEN << "Album list:" << RESET << '\n';
	std::cout << GREEN << "-----------" << RESET << '\n';
	for (const Album& album : albums)
		std::cout << GREEN << std::setw(5) << "* " << album << RESET;

}



// picture related functions //
void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture) const
{
	const int albumID = getAlbumID(albumName);

	if (doesPictureExistsInAlbum(albumName, picture.getName()))
		throw ItemAlreadyExistsException("Picture", picture.getName());

	const std::string addPictureToAlbumSQL =
		"INSERT INTO PICTURES (NAME, LOCATION, CREATION_DATE, ALBUM_ID) "
		"VALUES ('" + picture.getName() + "', '" + picture.getPath() +
		"', '" + picture.getCreationDate() + "'," + std::to_string(albumID) + ");";

	runSQL(addPictureToAlbumSQL);
}

void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) const
{
	const int albumID = getAlbumID(albumName);

	if (!doesPictureExistsInAlbum(albumName, pictureName))
		throw ItemNotFoundException("Picture", pictureName);

	const std::string addPictureToAlbumSQL = "DELETE FROM PICTURES WHERE ALBUM_ID = " + std::to_string(albumID) + " AND NAME = '" + pictureName + "';";
	runSQL(addPictureToAlbumSQL);
}

void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) const
{
	if (!doesAlbumExists(albumName))
		throw ItemNotFoundException("Album", albumName);

	if (!doesPictureExistsInAlbum(albumName, pictureName))
		throw ItemNotFoundException("Picture", pictureName);

	if (!doesUserExists(userId))
		throw ItemNotFoundException("User", std::to_string(userId));

	const int pictureID = getPictureID(albumName, pictureName);

	if (doesUserTaggedPicture(userId, pictureID))
		throw ItemAlreadyExistsException("User", std::to_string(userId));

	const std::string query = "INSERT INTO TAGS (PICTURE_ID, USER_ID) VALUES (" + std::to_string(pictureID) + ", " + std::to_string(userId) + ");";

	runSQL(query);
}

void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) const
{
	if (!doesAlbumExists(albumName))
		throw ItemNotFoundException("Album", albumName);

	if (!doesPictureExistsInAlbum(albumName, pictureName))
		throw ItemNotFoundException("Picture", pictureName);

	if (!doesUserExists(userId))
		throw ItemNotFoundException("User", std::to_string(userId));

	const int pictureID = getPictureID(albumName, pictureName);

	if (!doesUserTaggedPicture(userId, pictureID))
		throw ItemNotFoundException("Tag", std::to_string(userId));

	const std::string query = "DELETE FROM TAGS WHERE PICTURE_ID = " + std::to_string(pictureID) + " AND USER_ID = " + std::to_string(userId) + ";";
	runSQL(query);
}

int DatabaseAccess::getLastPictureId() const
{
	const std::string lastPicIDSql = "SELECT MAX(ID) FROM PICTURES;";
	int lastPicID = -1;

	runSQL(lastPicIDSql, &lastPicID, getIDCallback);

	if (lastPicID == -1)
		throw MyException("Failed to get last picture id.");

	return lastPicID;
}


// user related functions //
void DatabaseAccess::printUsers() const
{
	const std::list<User> users = getUsers();

	if (users.empty())
		throw MyException("There are no existing users.");

	std::cout << GREEN << "Users list:" << '\n' << RESET;
	std::cout << GREEN << "-----------" << '\n' << RESET;
	for (const auto& user : users)
		std::cout << GREEN << user << '\n' << RESET;

}

User DatabaseAccess::getUser(int userId) const
{
	const std::string getSingleUserSQL = "SELECT * FROM USERS WHERE ID = " + std::to_string(userId) + " LIMIT 1;";
	User user(-1, "");

	runSQL(getSingleUserSQL, &user, getUserCallback);

	if (user.getId() == -1 || user.getName().empty())
		throw ItemNotFoundException("User", userId);

	return user;
}

void DatabaseAccess::createUser(const User& user) const
{
	const std::string query = "INSERT INTO USERS (NAME) VALUES ('" + user.getName() + "');";
	runSQL(query);
}

void DatabaseAccess::deleteUser(const User& user) const
{
	if (!doesUserExists(user.getId()))
		throw ItemNotFoundException("User", user.getId());


	// delete the user from the users table
	const std::string deleteUserSQL = "DELETE FROM USERS WHERE ID = " + std::to_string(user.getId()) + ";";
	runSQL(deleteUserSQL);

	// delete all the tags associated with a user
	const std::string deleteUserTagsSQL = "DELETE FROM TAGS WHERE USER_ID = " + std::to_string(user.getId()) + ";";
	runSQL(deleteUserTagsSQL);

	// delete all the pictures associated with a user albums
	const std::string  deletePictureSQL = "DELETE FROM PICTURES WHERE ALBUM_ID IN (SELECT ID FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId()) + ");";
	runSQL(deletePictureSQL);

	// delete all the albums associated with a user
	const std::string deleteAlbumSQL = "DELETE FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId()) + ";";
	runSQL(deleteAlbumSQL);
}

bool DatabaseAccess::doesUserExists(int userId) const
{
	const std::string doesUserExistsSQL = "SELECT 1 FROM USERS WHERE ID = " + std::to_string(userId) + " LIMIT 1;";
	bool doesUserExists = false;

	runSQL(doesUserExistsSQL, &doesUserExists, existenceCallback);

	return doesUserExists;
}

int DatabaseAccess::getLastUserId() const
{
	const std::string lastUserIdSQL = "SELECT MAX(ID) FROM USERS;";
	int lastUserId = -1;

	runSQL(lastUserIdSQL, &lastUserId, getIDCallback);

	if (lastUserId == -1)
		throw MyException("Failed to get last user id.");

	return lastUserId;
}


// user statistics functions //
int DatabaseAccess::countAlbumsOwnedOfUser(const User& user) const
{
	if (!doesUserExists(user.getId()))
		throw ItemNotFoundException("User", user.getId());

	const std::string countAlbumsSQL = "SELECT COUNT(USER_ID) FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId()) + ";";
	int countAlbums = -1;

	runSQL(countAlbumsSQL, &countAlbums, countCallback);

	if (countAlbums == -1)
		throw MyException("Could not count albums of user " + std::to_string(user.getId()) + "!");


	return countAlbums;
}

int DatabaseAccess::countAlbumsTaggedOfUser(const User& user) const
{
	if (!doesUserExists(user.getId()))
		throw ItemNotFoundException("User", user.getId());

	// counts the number of albums that a user is tagged in one of their pictures
	const std::string countAlbumsSQL =
		"SELECT COUNT(DISTINCT ALBUMS.ID) FROM ALBUMS "
		"INNER JOIN PICTURES ON ALBUMS.ID = PICTURES.ALBUM_ID "
		"INNER JOIN TAGS ON PICTURES.ID = TAGS.PICTURE_ID "
		"WHERE TAGS.USER_ID = " + std::to_string(user.getId()) + ";";

	int countAlbums = -1;

	runSQL(countAlbumsSQL, &countAlbums, countCallback);

	if (countAlbums == -1)
		throw MyException("Could not count albums of user " + std::to_string(user.getId()) + "!");

	return countAlbums;
}

int DatabaseAccess::countTagsOfUser(const User& user) const
{
	if (!doesUserExists(user.getId()))
		throw ItemNotFoundException("User ", user.getId());


	const std::string countTagsSQL = "SELECT COUNT(PICTURE_ID) FROM TAGS WHERE USER_ID = " + std::to_string(user.getId()) + ";";
	int countTags = -1;

	runSQL(countTagsSQL, &countTags, countCallback);

	if (countTags == -1)
		throw MyException("Could not count tags of user " + std::to_string(user.getId()) + "!");


	return countTags;
}

float DatabaseAccess::averageTagsPerAlbumOfUser(const User& user) const
{
	const int albumsTaggedCount = countAlbumsTaggedOfUser(user);

	if (albumsTaggedCount == 0)
		return 0;


	return static_cast<float>(countTagsOfUser(user)) / static_cast<float>(albumsTaggedCount);
}




// tags related statistics functions //
User DatabaseAccess::getTopTaggedUser() const
{
	const std::string getTopUserSQL =
		"SELECT USERS.ID, USERS.NAME, COUNT(TAGS.USER_ID) AS TagCount FROM USERS "
		"INNER JOIN TAGS ON USERS.ID = TAGS.USER_ID "
		"GROUP BY USERS.ID, USERS.NAME "
		"ORDER BY TagCount DESC "
		"LIMIT 1;";

	User user(-1, "");

	runSQL(getTopUserSQL, &user, getUserCallback);

	if (user.getId() == -1)
		throw MyException("Could not get top tagged user!");

	return user;
}

Picture DatabaseAccess::getTopTaggedPicture() const
{
	// Construct the SQL query to find the picture that has been tagged the most
	const std::string query =
		"SELECT PICTURES.ID, PICTURES.NAME, PICTURES.LOCATION, PICTURES.CREATION_DATE, COUNT(TAGS.PICTURE_ID) AS TagCount "
		"FROM PICTURES "
		"INNER JOIN TAGS ON PICTURES.ID = TAGS.PICTURE_ID "
		"GROUP BY PICTURES.ID, PICTURES.NAME, PICTURES.LOCATION, PICTURES.CREATION_DATE "
		"ORDER BY TagCount DESC "
		"LIMIT 1;";

	Picture picture(-1, "");

	runSQL(query, &picture, getPictureCallback);

	if (picture.getId() == -1 || picture.getName().empty() || picture.getPath().empty() || picture.getCreationDate().empty())
		throw MyException("Could not found top tagged picture!");

	return picture;
}

std::list<Picture> DatabaseAccess::getTaggedPicturesOfUser(const User& user) const
{
	if (!doesUserExists(user.getId()))
		throw MyException("User " + std::to_string(user.getId()) + " does not exist!");

	const std::string getPicturesOfUserSQL =
		"SELECT PICTURES.ID, PICTURES.NAME, PICTURES.LOCATION, PICTURES.CREATION_DATE "
		"FROM PICTURES "
		"INNER JOIN TAGS ON PICTURES.ID = TAGS.PICTURE_ID "
		"WHERE TAGS.USER_ID = " + std::to_string(user.getId()) + ";";

	std::list<Picture> pictures;

	runSQL(getPicturesOfUserSQL, &pictures, getPicturesCallback);

	for (Picture& picture : pictures) {
		std::set<User> users_tagged = getPictureTags(picture);

		for (const auto& user_tagged : users_tagged)
			picture.tagUser(user_tagged);
	}

	return pictures;
}




// db access related functions //
bool DatabaseAccess::open()
{
	if (db != nullptr)
		return true;

	const int file_exist = _access(DB_NAME, 0);
	const int connection_successful = sqlite3_open(DB_NAME, &db);

	if (connection_successful != SQLITE_OK)
	{
		db = nullptr;
		std::cerr << "Error opening database\n";
		return false;
	}

	if (file_exist != 0)
	{
		// create schema, return false if failed
		try {
			constexpr const char* createUsersTable = "CREATE TABLE IF NOT EXISTS USERS ( ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, NAME TEXT NOT NULL );";
			runSQL(createUsersTable);

			constexpr const char* createAlbumsTable = "CREATE TABLE IF NOT EXISTS ALBUMS ( ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, NAME TEXT NOT NULL, USER_ID INTEGER NOT NULL, CREATION_DATE TEXT NOT NULL, FOREIGN KEY(USER_ID) REFERENCES USERS(ID) );";
			runSQL(createAlbumsTable);

			constexpr const char* createPicturesTable = "CREATE TABLE IF NOT EXISTS PICTURES  ( ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, NAME TEXT NOT NULL, LOCATION TEXT NOT NULL, CREATION_DATE TEXT NOT NULL, ALBUM_ID INTEGER NOT NULL, FOREIGN KEY(ALBUM_ID) REFERENCES ALBUMS(ID) );";
			runSQL(createPicturesTable);

			constexpr const char* createTagsTable = "CREATE TABLE IF NOT EXISTS TAGS ( PICTURE_ID INTEGER NOT NULL, USER_ID INTEGER NOT NULL, PRIMARY KEY(PICTURE_ID, USER_ID), FOREIGN KEY(PICTURE_ID) REFERENCES PICTURES(ID), FOREIGN KEY(USER_ID) REFERENCES USERS(ID) );";
			runSQL(createTagsTable);
		}
		catch (SqlException& e) {
			std::cerr << e.what() << '\n';
			sqlite3_close(db);
			return false;
		}
	}

	return true;
}

void DatabaseAccess::close()
{
	if (db == nullptr)
		return;
	sqlite3_close(db);
	db = nullptr;
}

void DatabaseAccess::clear() const
{
	constexpr const char* clearDB = "DELETE FROM USERS; DELETE FROM ALBUMS; DELETE FROM PICTURES; DELETE FROM TAGS; DELETE FROM SQLITE_SEQUENCE;";
	runSQL(clearDB);

	constexpr const char* vacuumDatabase = "VACUUM;";
	runSQL(vacuumDatabase);
}




// helper functions //
bool DatabaseAccess::doesPictureExistsInAlbum(const std::string& albumName, const std::string& pictureName) const
{
	const std::string doesPictureExistsSQL =
		"SELECT 1 FROM PICTURES "
		"INNER JOIN ALBUMS ON PICTURES.ALBUM_ID = ALBUMS.ID "
		"WHERE PICTURES.NAME = '" + pictureName + "' "
		"AND ALBUMS.NAME = '" + albumName + "' LIMIT 1;";

	bool doesPictureExists = false;

	runSQL(doesPictureExistsSQL, &doesPictureExists, existenceCallback);

	return doesPictureExists;
}

bool DatabaseAccess::doesAlbumExists(const std::string& albumName) const
{
	const std::string doesAlbumExistsSQL = "SELECT 1 FROM ALBUMS WHERE NAME = '" + albumName + "' LIMIT 1;";
	bool doesAlbumExist = false;

	runSQL(doesAlbumExistsSQL, &doesAlbumExist, existenceCallback);

	return doesAlbumExist;
}

bool DatabaseAccess::doesPictureExists(const std::string& pictureName, int pic_id) const
{
	const std::string doesPictureExistsSQL =
		"SELECT 1 FROM PICTURES "
		"WHERE PICTURES.NAME = '" + pictureName + "' "
		"AND PICTURES.ID = " + std::to_string(pic_id) +
		" LIMIT 1;";

	bool doesPictureExists = false;

	runSQL(doesPictureExistsSQL, &doesPictureExists, existenceCallback);

	return doesPictureExists;
}

std::list<User> DatabaseAccess::getUsers() const
{
	const std::string getAllUsersSQL = "SELECT * FROM USERS;";
	std::list<User> users;

	runSQL(getAllUsersSQL, &users, getUsersCallback);

	return users;
}

int DatabaseAccess::getAlbumID(const std::string& albumName) const
{
	const std::string getAlbumID = "SELECT ID FROM ALBUMS WHERE NAME = '" + albumName + "' LIMIT 1;";
	int albumID = -1;

	runSQL(getAlbumID, &albumID, getIDCallback);

	if (albumID == -1)
		throw ItemNotFoundException("Album", albumName);

	return albumID;
}

// this function gets only the album itself, not the pictures inside it
Album DatabaseAccess::getAlbum(const std::string& albumName, std::optional<int> userId) const
{
	Album album(-1, "");

	if (userId.has_value())
	{
		if (!doesAlbumExists(albumName, userId.value()))
			throw ItemNotFoundException("Album", albumName);


		const std::string getAlbumSQL = "SELECT * FROM ALBUMS WHERE NAME = '" + albumName + "' AND USER_ID = " + std::to_string(userId.value()) + " LIMIT 1;";

		runSQL(getAlbumSQL, &album, getAlbumCallback);

		if (album.getOwnerId() == -1 || album.getName().empty() || album.getCreationDate().empty())
			throw ItemNotFoundException("Album", userId.value());

		return album;
	}

	const std::string getAlbumSQL = "SELECT * FROM ALBUMS WHERE NAME = '" + albumName + "' LIMIT 1;";

	runSQL(getAlbumSQL, &album, getAlbumCallback);

	if (album.getOwnerId() == -1 || album.getName().empty() || album.getCreationDate().empty())
		throw ItemNotFoundException("Album", albumName);

	return album;
}

std::list<Picture> DatabaseAccess::getAlbumPictures(const Album& album) const
{
	if (!doesAlbumExists(album.getName(), album.getOwnerId()))
		throw ItemNotFoundException("Album", album.getName());

	const int album_id = getAlbumID(album.getName());
	const std::string getAlbumPicturesSQL = "SELECT * FROM PICTURES WHERE ALBUM_ID = " + std::to_string(album_id) + ";";
	std::list<Picture> pictures;

	runSQL(getAlbumPicturesSQL, &pictures, getPicturesCallback);

	for (auto& pic : pictures)
	{
		const auto picTags = getPictureTags(pic);
		for (const auto& tag : picTags)
			pic.tagUser(tag);
	}

	return pictures;
}

int DatabaseAccess::getPictureID(const std::string& albumName, const std::string& pictureName) const
{
	if (!doesAlbumExists(albumName))
		throw ItemNotFoundException("Album", albumName);

	const int albumID = getAlbumID(albumName);
	const std::string getPicIdSQL = "SELECT ID FROM PICTURES WHERE NAME = '" + pictureName + "' AND ALBUM_ID = " + std::to_string(albumID) + ";";

	int pictureID = -1;

	runSQL(getPicIdSQL, &pictureID, getIDCallback);

	if (pictureID == -1)
		throw ItemNotFoundException("Picture", pictureName);

	return pictureID;
}

std::set<User> DatabaseAccess::getPictureTags(const std::string& albumName, const Picture& picture) const
{
	if (!doesPictureExistsInAlbum(albumName, picture.getName()))
		throw ItemNotFoundException("Picture", picture.getName());

	const std::string getPicTagsSQL = "SELECT USER_ID FROM TAGS WHERE PICTURE_ID = " + std::to_string(picture.getId()) + ";";
	std::set<int> picTags;
	runSQL(getPicTagsSQL, &picTags, getTagsCallback);

	std::set<User> usersTagged;

	for (const int user_id : picTags)
		usersTagged.insert(getUser(user_id));

	return usersTagged;
}

std::set<User> DatabaseAccess::getPictureTags(const Picture& picture) const
{
	if (!doesPictureExists(picture.getName(), picture.getId()))
		throw ItemNotFoundException("Picture", picture.getName());

	const std::string getPicTagsSQL = "SELECT USER_ID FROM TAGS WHERE PICTURE_ID = " + std::to_string(picture.getId()) + ";";
	std::set<int> picTags;

	runSQL(getPicTagsSQL, &picTags, getTagsCallback);

	std::set<User> usersTagged;

	for (const int user_id : picTags)
		usersTagged.insert(getUser(user_id));

	return usersTagged;
}

bool DatabaseAccess::doesUserTaggedPicture(const int user_id, const int& pic_id) const
{
	const std::string doesUserTaggedPicSQL = "SELECT 1 FROM TAGS WHERE USER_ID = " + std::to_string(user_id) + " AND PICTURE_ID = " + std::to_string(pic_id) + " LIMIT 1;";
	bool doesUserTaggedPic = false;

	runSQL(doesUserTaggedPicSQL, &doesUserTaggedPic, existenceCallback);

	return doesUserTaggedPic;
}


// Wrapper functions for sqlite3_exec //
void DatabaseAccess::runSQL(const std::string& sql_statement) const
{
	if (db == nullptr)
		throw SqlException("Database is not open");

	char* errMessage = nullptr;
	const int res = sqlite3_exec(db, sql_statement.c_str(), nullptr, nullptr, &errMessage);

	if (res != SQLITE_OK)
		throw SqlException(errMessage);

}

void DatabaseAccess::runSQL(const std::string& sql_statement, void* data, int(*callback)(void*, int, char**, char**)) const
{
	if (db == nullptr)
		throw SqlException("Database is not open");

	char* errMessage = nullptr;
	const int res = sqlite3_exec(db, sql_statement.c_str(), callback, data, &errMessage);

	if (res != SQLITE_OK)
		throw SqlException(errMessage);

}
