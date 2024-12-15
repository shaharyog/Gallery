#include "JsonHelper.h"

json::value JsonHelper::usersToJson(const std::list<User>& users)
{
	json::value usersJson;
	int user_index = 0;

	for (const auto& user : users)
		usersJson[user_index++] = userToJson(user);
	
	return usersJson;
}


json::value JsonHelper::usersToJson(const std::set<User>& users)
{
	json::value usersJson;
	int user_index = 0;

	for (const auto& user : users)
		usersJson[user_index++] = userToJson(user);

	return usersJson;
}

json::value JsonHelper::albumsToJson(const std::list<Album>& albums)
{
	json::value albumsJson;
	int album_index = 0;

	for (const auto& album : albums)
		albumsJson[album_index++] = albumToJson(album);
	
	return albumsJson;
}

json::value JsonHelper::picturesToJson(const std::list<Picture>& pictures)
{
	json::value picturesJson;
	int picture_index = 0;

	for (const auto& picture : pictures)
		picturesJson[picture_index++] = pictureToJson(picture);

	return picturesJson;
}


json::value JsonHelper::userToJson(const User& user)
{
	json::value jsonUser;

	jsonUser[U("id")] = json::value::number(user.getId());
	jsonUser[U("name")] = json::value::string(utility::conversions::to_string_t(user.getName()));

	return jsonUser;
}

json::value JsonHelper::albumToJson(const Album& album)
{
	json::value jsonAlbum;
	jsonAlbum[U("owner_id")] = json::value::number(album.getOwnerId());
	jsonAlbum[U("owner_name")] = json::value::string(utility::conversions::to_string_t(album.getOwnerName()));
	jsonAlbum[U("name")] = json::value::string(utility::conversions::to_string_t(album.getName()));
	jsonAlbum[U("creation_date")] = json::value::string(utility::conversions::to_string_t(album.getCreationDate()));
	jsonAlbum[U("pictures_count")] = json::value::number(album.getPictures().size());

	return jsonAlbum;
}

json::value JsonHelper::pictureToJson(const Picture& picture)
{
	json::value jsonPicture;
	jsonPicture[U("id")] = json::value::number(picture.getId());
	jsonPicture[U("name")] = json::value::string(utility::conversions::to_string_t(picture.getName()));
	jsonPicture[U("path")] = json::value::string(utility::conversions::to_string_t(picture.getPath()));
	jsonPicture[U("creation_date")] = json::value::string(utility::conversions::to_string_t(picture.getCreationDate()));
	jsonPicture[U("tag_count")] = json::value::number(picture.getUsersTagged().size());

	return jsonPicture;
}
