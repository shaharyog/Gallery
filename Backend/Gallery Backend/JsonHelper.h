#pragma once
#include <cpprest/json.h>

#include "Album.h"

using namespace web;

class JsonHelper
{
public:
	// multiple objects to JSON array
	static json::value usersToJson(const std::list<User>& users);
	static json::value usersToJson(const std::set<User>& users);
	static json::value albumsToJson(const std::list<Album>& albums);
	static json::value picturesToJson(const std::list<Picture>& pictures);


	// single object to JSON
	static json::value userToJson(const User& user);
	static json::value albumToJson(const Album& album);
	static json::value pictureToJson(const Picture& picture);
};
