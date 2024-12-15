#include "GalleryAPI.h"

#include "Colors.h"
#include "ItemAlreadyExistsException.h"
#include "ItemNotFoundException.h"
#include "JsonHelper.h"


GalleryAPI::GalleryAPI(const std::string& uri)
{
	// construct the Gallery API URI
	const utility::string_t utilityUri = utility::conversions::to_string_t(uri);
	uri_builder uriBuilder(utilityUri);
	uriBuilder.append_path(U("/gallery/api"));

	// create an HTTP listener
	listener_ = http_listener(uriBuilder.to_uri());

	// support all needed HTTP methods
	listener_.support(methods::POST, [this](auto&& PH1) { handle_request(std::forward<decltype(PH1)>(PH1)); });
	listener_.support(methods::DEL, [this](auto&& PH1) { handle_request(std::forward<decltype(PH1)>(PH1)); });
	listener_.support(methods::GET, [this](auto&& PH1) { handle_request(std::forward<decltype(PH1)>(PH1)); });
}

const uri& GalleryAPI::listener_uri() const
{
	return listener_.uri();
}

void GalleryAPI::start()
{
	// start listening
	try {
		listener_.open().then([&]() {
			std::wcout << YELLOW << "Listening for requests at: " << listener_.uri().to_string() << RESET << '\n';
			}).wait();
	}
	catch (const std::exception& e) {
		std::cerr << RED << "Error: " << e.what() << RESET << '\n';
	}
}

void GalleryAPI::handle_request(const http_request& request) const
{
	const auto path = request.relative_uri().path();
	const auto method = request.method();

	if (method == methods::POST)
	{
		if (path == U("/create_album"))
		{
			create_album(request);
		}
		else if (path == U("/create_user"))
		{
			create_user(request);
		}
		else if (path == U("/add_picture_to_album"))
		{
			add_picture_to_album(request);
		}
		else if (path == U("/tag_user_in_picture"))
		{
			tag_user_in_picture(request);
		}
		else if (path == U("/get_albums_of_user"))
		{
			get_albums_of_user(request);
		}
		else if (path == U("/get_user"))
		{
			get_user(request);
		}
		else if (path == U("/get_user_albums_count"))
		{
			get_user_albums_count(request);
		}
		else if (path == U("/get_albums_tagged_user_count"))
		{
			get_albums_tagged_user_count(request);
		}
		else if (path == U("/get_count_tags_of_user"))
		{
			get_count_tags_of_user(request);
		}
		else if (path == U("/get_average_tags_of_user_per_album"))
		{
			get_average_tags_of_user_per_album(request);
		}
		else if (path == U("/get_album_pictures"))
		{
			get_album_pictures(request);
		}
		else if (path == U("/get_picture_tags"))
		{
			get_picture_tags(request);
		}
		else
		{
			request.reply(status_codes::NotFound);
		}
	}
	else if (method == methods::DEL)
	{
		if (path == U("/clear_db"))
		{
			clear_db(request);
		}
		if (path == U("/delete_user"))
		{
			delete_user(request);
		}
		else if (path == U("/delete_album"))
		{
			delete_album(request);
		}
		else if (path == U("/remove_picture_from_album"))
		{
			remove_picture_from_album(request);
		}
		else if (path == U("/untag_user_in_picture"))
		{
			untag_user_in_picture(request);
		}
		else
		{
			request.reply(status_codes::NotFound);
		}
	}
	else if (method == methods::GET)
	{
		if (path == U("/get_albums"))
		{
			get_albums(request);
		}
		else if (path == U("/get_users"))
		{
			get_users(request);
		}
		else
		{
			request.reply(status_codes::NotFound);
		}
	}
	else
	{
		request.reply(status_codes::MethodNotAllowed);
	}
}

void GalleryAPI::clear_db(const http_request& request) const
{
	try
	{
		std::cout << MAGENTA << "clear_db:" << GREEN << " Clearing database..." << RESET << '\n';
		db_.clear();
		std::cout << MAGENTA << "clear_db:" << GREEN << " Cleared Successfully!" << RESET << '\n';
		request.reply(status_codes::OK, "Database cleared successfully.");
	}
	catch (const std::exception& e)
	{
		std::cerr << MAGENTA << "clear_db:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
		request.reply(status_codes::InternalError, "Internal server error occurred.");
	}
}

void GalleryAPI::create_album(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		// Check if the JSON object contains the "name" field
		if (!requestBody.has_field(U("name")) || !requestBody.has_field(U("user_id")))
		{
			std::cout << MAGENTA << "create_album:" << RED << " Missing 'name' or 'user_id' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'name' or 'user_id' field in the request body.");
			return pplx::task_from_result();
		}

		const auto userId = requestBody.at(U("user_id")).as_integer();
		const auto albumName = utility::conversions::to_utf8string(requestBody.at(U("name")).as_string());

		const Album newAlbum(userId, albumName);

		db_.createAlbum(newAlbum);

		std::cout << MAGENTA << "create_album:" << GREEN << " Album created successfully." << RESET << '\n';
		return request.reply(status_codes::OK, "Album created successfully.");

	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "create_album:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "create_album:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "create_album:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::create_user(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		// Check if the JSON object contains the "name" field
		if (!requestBody.has_field(U("name")))
		{
			std::cout << MAGENTA << "create_user:" << RED << " Missing 'name' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'name' field in the request body.");
			return pplx::task_from_result();
		}

		// parse the JSON object
		const auto userName = utility::conversions::to_utf8string(requestBody.at(U("name")).as_string());
		const User newUser(-1, userName);

		db_.createUser(newUser);

		std::cout << MAGENTA << "create_user:" << GREEN << " User created successfully." << RESET << '\n';
		return request.reply(status_codes::OK, "User created successfully.");

	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "create_user:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "create_user:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "create_user:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::add_picture_to_album(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		if (!requestBody.has_field(U("album_name")) || !requestBody.has_field(U("picture_name")) ||
			!requestBody.has_field(U("path")))
		{
			std::cout << MAGENTA << "add_picture_to_album:" << RED << " Missing 'album_name', 'picture_name', or 'path' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'album_name', 'picture_name', or 'path' field in the request body.");
			return pplx::task_from_result();
		}

		const auto albumName = utility::conversions::to_utf8string(requestBody.at(U("album_name")).as_string());
		const auto pictureName = utility::conversions::to_utf8string(requestBody.at(U("picture_name")).as_string());
		const auto picturePath = utility::conversions::to_utf8string(requestBody.at(U("path")).as_string());

		Picture new_picture(-1, pictureName);
		new_picture.setPath(picturePath);

		db_.addPictureToAlbumByName(albumName, new_picture);

		std::cout << MAGENTA << "add_picture_to_album:" << GREEN << " Picture added to album successfully." << RESET << '\n';
		return request.reply(status_codes::OK, "Picture added to album successfully.");
	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "add_picture_to_album:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "add_picture_to_album:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "add_picture_to_album:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::tag_user_in_picture(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		if (!requestBody.has_field(U("album_name")) || !requestBody.has_field(U("picture_name")) || !requestBody.has_field(U("user_id")))
		{
			std::cout << MAGENTA << "tag_user_in_picture:" << RED << " Missing 'album_name', 'picture_name', or 'user_id' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'album_name', 'picture_name', or 'user_id' field in the request body.");
			return pplx::task_from_result();
		}

		const auto albumName = utility::conversions::to_utf8string(requestBody.at(U("album_name")).as_string());
		const auto pictureName = utility::conversions::to_utf8string(requestBody.at(U("picture_name")).as_string());
		const auto userId = requestBody.at(U("user_id")).as_integer();

		// Call a function to tag the user in the picture
		db_.tagUserInPicture(albumName, pictureName, userId);

		std::cout << MAGENTA << "tag_user_in_picture:" << GREEN << " User tagged in picture successfully." << RESET << '\n';
		return request.reply(status_codes::OK, "User tagged in picture successfully.");
	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "tag_user_in_picture:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "tag_user_in_picture:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "tag_user_in_picture:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::delete_user(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		// Check if the JSON object contains the "name" field
		if (!requestBody.has_field(U("id")))
		{
			std::cout << MAGENTA << "delete_user:" << RED << " Missing 'id' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'id' field in the request body.");
			return pplx::task_from_result();
		}

		// parse the JSON object
		const auto userId = requestBody.at(U("id")).as_integer();

		const User userToDelete(userId, "");

		db_.deleteUser(userToDelete);

		std::cout << MAGENTA << "delete_user:" << GREEN << " User deleted successfully." << RESET << '\n';
		return request.reply(status_codes::OK, "User deleted successfully.");

	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "delete_user:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "delete_user:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "delete_user:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::delete_album(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		// Check if the JSON object contains the "name" field
		if (!requestBody.has_field(U("name")) || !requestBody.has_field(U("user_id")))
		{
			std::cout << MAGENTA << "delete_album:" << RED << " Missing 'name' or 'user_id' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'name' or 'user_id' field in the request body.");
			return pplx::task_from_result();
		}

		const auto userId = requestBody.at(U("user_id")).as_integer();
		const auto albumName = utility::conversions::to_utf8string(requestBody.at(U("name")).as_string());

		db_.deleteAlbum(albumName, userId);

		std::cout << MAGENTA << "delete_album:" << GREEN << " Album deleted successfully." << RESET << '\n';
		return request.reply(status_codes::OK, "Album deleted successfully.");

	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "delete_album:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "delete_album:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "delete_album:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::remove_picture_from_album(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		if (!requestBody.has_field(U("album_name")) || !requestBody.has_field(U("picture_name")))
		{
			std::cout << MAGENTA << "remove_picture_from_album:" << RED << " Missing 'album_name' or 'picture_name' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'album_name' or 'picture_name' field in the request body.");
			return pplx::task_from_result();
		}

		const auto albumName = utility::conversions::to_utf8string(requestBody.at(U("album_name")).as_string());
		const auto pictureName = utility::conversions::to_utf8string(requestBody.at(U("picture_name")).as_string());

		// Call a function to remove the picture from the album
		db_.removePictureFromAlbumByName(albumName, pictureName);

		std::cout << MAGENTA << "remove_picture_from_album:" << GREEN << " Picture removed from album successfully." << RESET << '\n';
		return request.reply(status_codes::OK, "Picture removed from album successfully.");
	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "remove_picture_from_album:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "remove_picture_from_album:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "remove_picture_from_album:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::untag_user_in_picture(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		if (!requestBody.has_field(U("album_name")) || !requestBody.has_field(U("picture_name")) || !requestBody.has_field(U("user_id")))
		{
			std::cout << MAGENTA << "untag_user_in_picture:" << RED << " Missing 'album_name', 'picture_name', or 'user_id' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'album_name', 'picture_name', or 'user_id' field in the request body.");
			return pplx::task_from_result();
		}

		const auto albumName = utility::conversions::to_utf8string(requestBody.at(U("album_name")).as_string());
		const auto pictureName = utility::conversions::to_utf8string(requestBody.at(U("picture_name")).as_string());
		const auto userId = requestBody.at(U("user_id")).as_integer();

		// Call a function to untag the user from the picture
		db_.untagUserInPicture(albumName, pictureName, userId);

		std::cout << MAGENTA << "untag_user_in_picture:" << GREEN << " User untagged from picture successfully." << RESET << '\n';
		return request.reply(status_codes::OK, "User untagged from picture successfully.");
	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "untag_user_in_picture:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "untag_user_in_picture:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "untag_user_in_picture:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::get_albums(const http_request& request) const
{
	try
	{
		// Retrieve albums from the database
		const std::list<Album> albums = db_.getAlbums();

		const auto albumsJson = JsonHelper::albumsToJson(albums);

		std::cout << MAGENTA << "get_albums:" << GREEN << " Albums retrieved successfully and parsed to JSON." << RESET << '\n';
		request.reply(status_codes::OK, albumsJson);
	}
	catch (const ItemAlreadyExistsException& e)
	{
		std::cout << MAGENTA << "get_albums:" << RED << e.what() << RESET << '\n';
		request.reply(status_codes::Conflict, e.what());
	}
	catch (const ItemNotFoundException& e)
	{
		std::cout << MAGENTA << "get_albums:" << RED << e.what() << RESET << '\n';
		request.reply(status_codes::NotFound, e.what());
	}
	catch (const std::exception& e)
	{
		std::cerr << MAGENTA << "get_albums:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
		request.reply(status_codes::InternalError, "Internal server error occurred.");
	}
}

void GalleryAPI::get_albums_of_user(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		// Check if the JSON object contains the "name" field
		if (!requestBody.has_field(U("id")))
		{
			std::cout << MAGENTA << "get_albums_of_user:" << RED << " Missing 'id' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'id' field in the request body.");
			return pplx::task_from_result();
		}

		const auto userId = requestBody.at(U("id")).as_integer();
		const User user(userId, "");

		const std::list<Album> albums = db_.getAlbumsOfUser(user);

		const auto userAlbumsJson = JsonHelper::albumsToJson(albums);

		std::cout << MAGENTA << "get_albums_of_user:" << GREEN << " Albums of user retrieved successfully and parsed to JSON." << RESET << '\n';
		return request.reply(status_codes::OK, userAlbumsJson);

	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "get_albums_of_user:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "get_albums_of_user:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "get_albums_of_user:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::get_users(const http_request& request) const
{
	try
	{
		// Retrieve albums from the database
		const std::list<User> users = db_.getUsers();

		const auto usersJson = JsonHelper::usersToJson(users);

		std::cout << MAGENTA << "get_users:" << GREEN << " Users retrieved successfully and parsed to JSON." << RESET << '\n';
		request.reply(status_codes::OK, usersJson);
	}
	catch (const ItemAlreadyExistsException& e)
	{
		std::cout << MAGENTA << "get_users:" << RED << e.what() << RESET << '\n';
		request.reply(status_codes::Conflict, e.what());
	}
	catch (const ItemNotFoundException& e)
	{
		std::cout << MAGENTA << "get_users:" << RED << e.what() << RESET << '\n';
		request.reply(status_codes::NotFound, e.what());
	}
	catch (const std::exception& e)
	{
		std::cerr << MAGENTA << "get_users:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
		request.reply(status_codes::InternalError, "Internal server error occurred.");
	}
}

void GalleryAPI::get_user(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		// Check if the JSON object contains the "name" field
		if (!requestBody.has_field(U("id")))
		{
			std::cout << MAGENTA << "get_user:" << RED << " Missing 'id' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'id' field in the request body.");
			return pplx::task_from_result();
		}

		const auto userId = requestBody.at(U("id")).as_integer();

		const User user = db_.getUser(userId);

		const auto userJson = JsonHelper::userToJson(user);

		std::cout << MAGENTA << "get_user:" << GREEN << " User retrieved successfully and parsed to JSON." << RESET << '\n';
		return request.reply(status_codes::OK, userJson);

	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "get_user:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "get_user:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "get_user:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::get_user_albums_count(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		// Check if the JSON object contains the "name" field
		if (!requestBody.has_field(U("id")))
		{
			std::cout << MAGENTA << "get_user_albums_count:" << RED << " Missing 'id' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'id' field in the request body.");
			return pplx::task_from_result();
		}

		const auto userId = requestBody.at(U("id")).as_integer();
		const User user(userId, "");
		const int count = db_.countAlbumsOwnedOfUser(user);

		const auto countJson = json::value::number(count);

		std::cout << MAGENTA << "get_user_albums_count:" << GREEN << " User albums count retrieved successfully and parsed to JSON." << RESET << '\n';
		return request.reply(status_codes::OK, countJson);

	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "get_user_albums_count:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "get_user_albums_count:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "get_user_albums_count:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::get_albums_tagged_user_count(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		// Check if the JSON object contains the "name" field
		if (!requestBody.has_field(U("id")))
		{
			std::cout << MAGENTA << "get_albums_tagged_user_count:" << RED << " Missing 'id' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'id' field in the request body.");
			return pplx::task_from_result();
		}

		const auto userId = requestBody.at(U("id")).as_integer();
		const User user(userId, "");
		const int count = db_.countAlbumsTaggedOfUser(user);

		const auto countJson = json::value::number(count);

		std::cout << MAGENTA << "get_albums_tagged_user_count:" << GREEN << " Albums tagged user count retrieved successfully and parsed to JSON." << RESET << '\n';																
		return request.reply(status_codes::OK, countJson);

	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "get_albums_tagged_user_count:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "get_albums_tagged_user_count:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "get_albums_tagged_user_count:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::get_count_tags_of_user(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		// Check if the JSON object contains the "name" field
		if (!requestBody.has_field(U("id")))
		{
			std::cout << MAGENTA << "get_count_tags_of_user:" << RED << " Missing 'id' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'id' field in the request body.");
			return pplx::task_from_result();
		}

		const auto userId = requestBody.at(U("id")).as_integer();
		const User user(userId, "");
		const int count = db_.countTagsOfUser(user);

		const auto countJson = json::value::number(count);

		std::cout << MAGENTA << "get_count_tags_of_user:" << GREEN << " Count tags of user retrieved successfully and parsed to JSON." << RESET << '\n';																																
		return request.reply(status_codes::OK, countJson);

	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "get_count_tags_of_user:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "get_count_tags_of_user:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "get_count_tags_of_user:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::get_average_tags_of_user_per_album(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		// Check if the JSON object contains the "name" field
		if (!requestBody.has_field(U("id")))
		{
			std::cout << MAGENTA << "get_average_tags_of_user_per_album:" << RED << " Missing 'id' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'id' field in the request body.");
			return pplx::task_from_result();
		}

		const auto userId = requestBody.at(U("id")).as_integer();
		const User user(userId, "");
		const float average = db_.averageTagsPerAlbumOfUser(user);

		const auto averageJson = json::value::number(average);

		std::cout << MAGENTA << "get_average_tags_of_user_per_album:" << GREEN << " Average tags of user per album retrieved successfully and parsed to JSON." << RESET << '\n';
		return request.reply(status_codes::OK, averageJson);

	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "get_average_tags_of_user_per_album:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "get_average_tags_of_user_per_album:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "get_average_tags_of_user_per_album:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::get_album_pictures(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		// Check if the JSON object contains the "name" field
		if (!requestBody.has_field(U("owner_id")) || !requestBody.has_field(U("album_name")))
		{
			std::cout << MAGENTA << "get_album_pictures:" << RED << " Missing 'owner_id' or 'album_name' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'owner_id' or 'album_name' field in the request body.");
			return pplx::task_from_result();
		}


		const auto ownerId = requestBody.at(U("owner_id")).as_integer();
		const auto albumName = utility::conversions::to_utf8string(requestBody.at(U("album_name")).as_string());

		const Album album(ownerId, albumName);
		const std::list<Picture> album_pictures = db_.getAlbumPictures(album);

		const auto picturesJson = JsonHelper::picturesToJson(album_pictures);

		std::cout << MAGENTA << "get_album_pictures:" << GREEN << " Album pictures retrieved successfully and parsed to JSON." << RESET << '\n';
		return request.reply(status_codes::OK, picturesJson);

	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "get_album_pictures:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "get_album_pictures:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "get_album_pictures:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}

void GalleryAPI::get_picture_tags(const http_request& request) const
{
	request.extract_json().then([request, this](json::value requestBody)
	{
		// Check if the JSON object contains the "name" field
		if (!requestBody.has_field(U("id")) || !requestBody.has_field(U("name")))
		{
			std::cout << MAGENTA << "get_picture_tags:" << RED << " Missing 'id' or 'name' field in the request body." << RESET << '\n';
			request.reply(status_codes::BadRequest, "Missing 'id' or 'name' field in the request body.");
			return pplx::task_from_result();
		}


		const auto picID = requestBody.at(U("id")).as_integer();
		const auto picName = utility::conversions::to_utf8string(requestBody.at(U("name")).as_string());

		const Picture pic(picID, picName);
		const auto picTags = db_.getPictureTags(pic);

		const auto picTagsJson = JsonHelper::usersToJson(picTags);

		std::cout << MAGENTA << "get_picture_tags:" << GREEN << " Picture tags retrieved successfully and parsed to JSON." << RESET << '\n';
		return request.reply(status_codes::OK, picTagsJson);

	}).then([=](const pplx::task<void>& t)
	{
		try
		{
			t.get();
		}
		catch (const ItemAlreadyExistsException& e)
		{
			std::cout << MAGENTA << "get_picture_tags:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::Conflict, e.what());
		}
		catch (const ItemNotFoundException& e)
		{
			std::cout << MAGENTA << "get_picture_tags:" << RED << e.what() << RESET << '\n';
			request.reply(status_codes::NotFound, e.what());
		}
		catch (const std::exception& e)
		{
			std::cout << MAGENTA << "get_picture_tags:" << RED << " Internal server error occurred: " << e.what() << RESET << '\n';
			request.reply(status_codes::InternalError, "Internal server error occurred.");
		}
	});
}
