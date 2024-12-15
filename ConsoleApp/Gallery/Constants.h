#pragma once
#include <string>
#include <vector>

enum CommandType
{
	HELP = 0,
	// Album operations
	CREATE_ALBUM = 1,
	OPEN_ALBUM,
	CLOSE_ALBUM,
	DELETE_ALBUM,
	LIST_ALBUMS,
	LIST_ALBUMS_OF_USER,

	// Picture operations
	ADD_PICTURE,
	REMOVE_PICTURE,
	SHOW_PICTURE,
	LIST_PICTURES,
	TAG_USER,
	UNTAG_USER,
	LIST_TAGS,

	// User operations
	ADD_USER,
	REMOVE_USER,
	LIST_OF_USER,
	USER_STATISTICS,


	TOP_TAGGED_USER,
	TOP_TAGGED_PICTURE,
	PICTURES_TAGGED_USER,

	// BONUS operations
	EDIT_PICTURE_PERMISSIONS,
	COPY_PICTURE,

	EXIT = 99
};

struct CommandPrompt {
	CommandType type;
	const std::string prompt;
};

struct CommandGroup {
	const std::string title;
	const std::vector<CommandPrompt> commands;
};


constexpr const char* DB_NAME = "galleryDB.sqlite";

constexpr int FIRST_USER_ID = 200;
constexpr int FIRST_PICTURE_ID = 100;

enum class PhotoViewApp
{
	PAINT = 0,
	IRFANVIEW = 1
};

constexpr const char* APP_PATHS[] = {
	R"(mspaint)",									// Index 0: MS Paint
	R"(C:\Program Files\IrfanView\i_view64.exe)"	// Index 1: IrfanView
};


enum class Permissions
{
	READ_ONLY,
	WRITEABLE
};