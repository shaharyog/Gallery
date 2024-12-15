#include "CallbackFuncs.h"
#include <list>
#include "Album.h"
#include "User.h"


// general callbacks //
int existenceCallback(void* data, int argc, char** argv, char** azColName)
{
	bool* existenceFlagPtr = static_cast<bool*>(data);
	*existenceFlagPtr = true; // Set exists to true if the result is not empty
	return 0;
}
int countCallback(void* data, int argc, char** argv, char** azColName)
{
	int* countAlbums = static_cast<int*>(data);

	if (argv[0] != nullptr)
		*countAlbums = std::stoi(argv[0]); // Parse the count from the result

	return 0;
}
int getIDCallback(void* data, int argc, char** argv, char** azColName)
{
	int* IDPtr = static_cast<int*>(data);

	if (argv[0] != nullptr)
		*IDPtr = std::stoi(argv[0]);

	return 0;
}



// album related callbacks //
int getAlbumCallback(void* data, int argc, char** argv, char** azColName)
{
	Album* album = static_cast<Album*>(data);

	for (int i = 0; i < argc; i++) {
		if (argv[i] == nullptr)
			continue;

		if (std::string(azColName[i]) == "NAME")
			album->setName(argv[i]);
		else if (std::string(azColName[i]) == "USER_ID")
			album->setOwner(std::stoi(argv[i]));
		else if (std::string(azColName[i]) == "CREATION_DATE")
			album->setCreationDate(argv[i]);
	}

	return 0;
}
int getAlbumsCallback(void* data, int argc, char** argv, char** azColName)
{
	std::list<Album>* albums = static_cast<std::list<Album>*>(data);
	Album album;

	for (int i = 0; i < argc; i++)
	{
		if (argv[i] == nullptr)
			continue;

		if (std::string(azColName[i]) == "USER_ID")
			album.setOwner(std::stoi(argv[i]));
		else if (std::string(azColName[i]) == "NAME")
			album.setName(argv[i]);
		else if (std::string(azColName[i]) == "CREATION_DATE")
			album.setCreationDate(argv[i]);
	}

	// missing data
	if (album.getOwnerId() == 0 || album.getCreationDate().empty())
		return 0;

	albums->push_back(album);

	return 0;

}



// user related callbacks //
int getUsersCallback(void* data, int argc, char** argv, char** azColName)
{
	std::list<User>* users = static_cast<std::list<User>*>(data);
	User user(-1, "");

	for (int i = 0; i < argc; i++)
	{
		if (argv[i] == nullptr)
			continue;

		if (std::string(azColName[i]) == "ID")
			user.setId(std::stoi(argv[i]));
		else if (std::string(azColName[i]) == "NAME")
			user.setName(argv[i]);
	}

	// missing data
	if (user.getId() == -1)
		return 0;

	users->push_back(user);

	return 0;
}
int getUserCallback(void* data, int argc, char** argv, char** azColName)
{
	User* user = static_cast<User*>(data);

	for (int i = 0; i < argc; i++)
	{
		if (argv[i] == nullptr)
			continue;

		if (std::string(azColName[i]) == "ID")
			user->setId(std::stoi(argv[i]));
		else if (std::string(azColName[i]) == "NAME")
			user->setName(argv[i]);
	}

	return 0;
}



// picture related callbacks //
int getPictureCallback(void* data, int argc, char** argv, char** azColName)
{
	Picture* picture = static_cast<Picture*>(data);

	for (int i = 0; i < argc; i++)
	{
		if (argv[i] == nullptr)
			continue;

		if (std::string(azColName[i]) == "ID")
			picture->setId(std::stoi(argv[i]));
		else if (std::string(azColName[i]) == "NAME")
			picture->setName(argv[i]);
		else if (std::string(azColName[i]) == "LOCATION")
			picture->setPath(argv[i]);
		else if (std::string(azColName[i]) == "CREATION_DATE")
			picture->setCreationDate(argv[i]);
	}

	return 0;
}
int getPicturesCallback(void* data, int argc, char** argv, char** azColName)
{
	std::list<Picture>* pictures = static_cast<std::list<Picture>*>(data);
	Picture picture(-1, "");

	for (int i = 0; i < argc; i++)
	{
		if (argv[i] == nullptr)
			continue;

		if (std::string(azColName[i]) == "ID")
			picture.setId(std::stoi(argv[i]));
		else if (std::string(azColName[i]) == "NAME")
			picture.setName(argv[i]);
		else if (std::string(azColName[i]) == "LOCATION")
			picture.setPath(argv[i]);
		else if (std::string(azColName[i]) == "CREATION_DATE")
			picture.setCreationDate(argv[i]);
	}

	if (picture.getId() == -1 || picture.getCreationDate().empty())
		return 0;

	pictures->push_back(picture);

	return 0;
}



// tag related callbacks //
int getTagsCallback(void* data, int argc, char** argv, char** azColName)
{
	std::set<int>* tags = static_cast<std::set<int>*>(data);

	if (argv[0] != nullptr)
	{
		tags->insert(std::stoi(argv[0]));
	}

	return 0;
}

