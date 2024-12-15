#include "DataAccessTest.h"

#include <random>

#include "Colors.h"
#include "Constants.h"

void DataAccessTest::createTables()
{
	_dbAccess.open();	// open database (create tables if not exists)
	_dbAccess.close();
}


void DataAccessTest::addData()
{
	if (!_dbAccess.open())
	{
		std::cerr << RED << "Failed to open database." << RESET << '\n';
		return;
	}

	std::list<User> users = {
		{1, "shahar" },
		{2, "dov" },
		{3, "itamar" }
	};


	try
	{
		int counter = 1;
		for (auto& user : users)
		{
			_dbAccess.createUser(user);

			Album user_album(user.getId(), user.getName() + "'s album");
			_dbAccess.createAlbum(user_album);

			std::list<Picture> pics = {
				{ counter, "pic" + std::to_string(counter) },
				{ counter + 1, "pic" + std::to_string(counter + 1) }
			};

			for (auto& pic : pics)
			{
				_dbAccess.addPictureToAlbumByName(user_album.getName(), pic);


				std::vector userIDs = { 1, 2, 3 }; // IDs of available users

				// Shuffle the user IDs
				std::random_device rd;
				std::mt19937 g(rd());
				std::shuffle(userIDs.begin(), userIDs.end(), g);

				// Tag two random users
				for (int i = 0; i < 2; ++i) {
					_dbAccess.tagUserInPicture(user_album.getName(),pic.getName(), userIDs[i]);
				}
			}

			counter += 2;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	_dbAccess.close();
}

void DataAccessTest::changeData()
{
	if (!_dbAccess.open())
	{
		std::cerr << RED << "Failed to open database." << RESET << '\n';
		return;
	}

	const Album myPrivateAlbum(4, "my private album");
	const Picture myPrivatePicture(7, "My Femily");  // Add picture with grammar mistake
	User myPrivateUser(4, "my private user");

	try
	{
		_dbAccess.createUser(myPrivateUser);
		_dbAccess.createAlbum(myPrivateAlbum);
		_dbAccess.addPictureToAlbumByName(myPrivateAlbum.getName(), myPrivatePicture);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	_dbAccess.close();
	
	// manually open db
	sqlite3* db = nullptr;
	const int res = sqlite3_open(DB_NAME, &db);

	if (res != SQLITE_OK)
	{
		std::cerr << RED <<  "Error opening database\n" << RESET << '\n';
		return;
	}

	// manually change data
	const char* sqlStatement = "UPDATE PICTURES SET NAME = 'My Family' WHERE ID = 7";
	char** errMsg = nullptr;

	const int query_res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, errMsg);
	if (query_res != SQLITE_OK)
	{
		std::cerr << RED << "Error changing data\n" << RESET << '\n';
	}
	
	sqlite3_close(db);

}

void DataAccessTest::deleteData()
{
	if (!_dbAccess.open())
	{
		std::cerr << RED << "Failed to open database." << RESET << '\n';
		return;
	}

	const User shahar(1, "shahar");

	_dbAccess.deleteUser(shahar);

	_dbAccess.close();
}
