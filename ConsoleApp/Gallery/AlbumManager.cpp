#include "AlbumManager.h"
#include <iostream>
#include "Constants.h"
#include "MyException.h"
#include "AlbumNotOpenException.h"
#include "Colors.h"
#include "ImageAccess.h"


AlbumManager::AlbumManager(IDataAccess& dataAccess) :
	m_nextPictureId(FIRST_PICTURE_ID), m_nextUserId(FIRST_USER_ID), m_dataAccess(dataAccess)
{
	// Left empty
	m_dataAccess.open();
	try
	{
		m_nextUserId = m_dataAccess.getLastUserId();
		m_nextPictureId = m_dataAccess.getLastPictureId();
	} catch (const MyException&) {
		// swallow
	}
}

void AlbumManager::executeCommand(CommandType command) {
	try {
		const handler_func_t handler = m_commands.at(command);
		(this->*handler)();
	}
	catch (const std::out_of_range&) {
		throw MyException("Error: Invalid command number [" + std::to_string(command) + "]\n");
	}
}

void AlbumManager::printHelp()
{
	std::cout << YELLOW << "Supported Gallery commands:" << RESET << '\n';
	std::cout << YELLOW << "===========================" << RESET << "\n\n";

	for (const CommandGroup& group : m_prompts) {
		std::cout << MAGENTA << group.title << RESET << "\n";
		std::string space(".  ");
		for (const CommandPrompt& command : group.commands) {
			space = command.type < 10 ? ".   " : ".  ";

			std::cout << BLUE << command.type << space << command.prompt << RESET << '\n';
		}
		std::cout << '\n';
	}
}


// ******************* Album ******************* 
void AlbumManager::createAlbum()
{
	const std::string userIdStr = getInputFromConsole("Enter user id: ");
	const int userId = std::stoi(userIdStr);

	if (!m_dataAccess.doesUserExists(userId))
		throw MyException("Error: Can't create album since there is no user with id [" + userIdStr + "]\n");


	const std::string name = getInputFromConsole("Enter album name - ");
	if (m_dataAccess.doesAlbumExists(name, userId))
		throw MyException("Error: Failed to create album, album with the same name already exists\n");


	const Album newAlbum(userId, name);
	m_dataAccess.createAlbum(newAlbum);

	std::cout << GREEN << "Album [" << newAlbum.getName() << "] created successfully by user@" << newAlbum.getOwnerId()
		<< RESET << '\n';
}

void AlbumManager::openAlbum()
{
	if (isCurrentAlbumSet())
		closeAlbum();


	const std::string userIdStr = getInputFromConsole("Enter user id: ");
	const int userId = std::stoi(userIdStr);
	if (!m_dataAccess.doesUserExists(userId))
		throw MyException("Error: Can't open album since there is no user with id @" + userIdStr + ".\n");


	const std::string name = getInputFromConsole("Enter album name - ");
	if (!m_dataAccess.doesAlbumExists(name, userId))
		throw MyException("Error: Failed to open album, since there is no album with name:" + name + ".\n");


	m_openAlbum = m_dataAccess.openAlbum(name);
	m_currentAlbumName = name;
	// success
	std::cout << GREEN << "Album [" << name << "] opened successfully.\n" << RESET;
}

void AlbumManager::closeAlbum()
{
	refreshOpenAlbum();

	std::cout << GREEN << "Album [" << m_openAlbum.getName() << "] closed successfully.\n" << RESET;
	m_dataAccess.closeAlbum(m_openAlbum);
	m_currentAlbumName = "";
}

void AlbumManager::deleteAlbum()
{
	const std::string userIdStr = getInputFromConsole("Enter user id: ");
	const int userId = std::stoi(userIdStr);
	if (!m_dataAccess.doesUserExists(userId))
		throw MyException("Error: There is no user with id @" + userIdStr + "\n");


	const std::string albumName = getInputFromConsole("Enter album name - ");
	if (!m_dataAccess.doesAlbumExists(albumName, userId))
		throw MyException("Error: Failed to delete album, since there is no album with name:" + albumName + ".\n");


	// album exist, close album if it is opened
	if (isCurrentAlbumSet() &&
		(m_openAlbum.getOwnerId() == userId && m_openAlbum.getName() == albumName)) {

		closeAlbum();
	}

	m_dataAccess.deleteAlbum(albumName, userId);
	std::cout << GREEN << "Album [" << albumName << "] @" << userId << " deleted successfully." << RESET << '\n';
}

void AlbumManager::listAlbums()
{
	m_dataAccess.printAlbums();
}

void AlbumManager::listAlbumsOfUser()
{
	const std::string userIdStr = getInputFromConsole("Enter user id: ");
	const int userId = std::stoi(userIdStr);

	if (!m_dataAccess.doesUserExists(userId))
		throw MyException("Error: There is no user with id @" + userIdStr + "\n");


	const User& user = m_dataAccess.getUser(userId);
	const std::list<Album>& albums = m_dataAccess.getAlbumsOfUser(user);

	if (albums.empty())
		throw MyException("Error: user@" + std::to_string(userId) + " has no albums.\n");

	std::cout << GREEN << "Albums list of user@" << user.getId() << RESET << ":\n";
	std::cout << GREEN << "------------------------\n" << RESET;

	for (const auto& album : albums)
		std::cout << GREEN << "   + [" << album.getName() << "] - created on " << album.getCreationDate() << RESET << '\n';

}


// ******************* Picture ******************* 
void AlbumManager::addPictureToAlbum()
{
	refreshOpenAlbum();

	const std::string picName = getInputFromConsole("Enter picture name: ");
	if (m_openAlbum.doesPictureExists(picName))
		throw MyException("Error: Failed to add picture, picture with the same name already exists.\n");

	Picture picture(++m_nextPictureId, picName);
	const std::string picPath = getInputFromConsole("Enter picture path: ");
	picture.setPath(picPath);

	m_dataAccess.addPictureToAlbumByName(m_openAlbum.getName(), picture);

	std::cout << GREEN <<  "Picture [" << picture.getId() << "] successfully added to Album [" << m_openAlbum.getName() << "].\n" << RESET;
}

void AlbumManager::removePictureFromAlbum()
{
	refreshOpenAlbum();

	const std::string picName = getInputFromConsole("Enter picture name: ");
	if (!m_openAlbum.doesPictureExists(picName))
		throw MyException("Error: There is no picture with name <" + picName + ">.\n");

	const auto picture = m_openAlbum.getPicture(picName);
	m_dataAccess.removePictureFromAlbumByName(m_openAlbum.getName(), picture.getName());
	std::cout << GREEN << "Picture <" << picName << "> successfully removed from Album [" << m_openAlbum.getName() << "].\n" << RESET;
}

void AlbumManager::listPicturesInAlbum()
{
	refreshOpenAlbum();

	const std::list<Picture>& albumPictures = m_openAlbum.getPictures();

	if (albumPictures.empty())
		throw MyException("Error: There are no pictures in Album [" + m_openAlbum.getName() + "].\n");

	std::cout << GREEN << "List of pictures in Album [" << m_openAlbum.getName()
		<< "] of user@" << m_openAlbum.getOwnerId() << ":\n" << RESET;

	for (const auto& pic : albumPictures) {
		std::cout << GREEN << "   + Picture [" << pic.getId() << "] - " << pic.getName() <<
			"\tLocation: [" << pic.getPath() << "]\tCreation Date: [" <<
			pic.getCreationDate() << "]\tTags: [" << pic.getTagsCount() << "]\n" << RESET;
	}
	std::cout << '\n';
}

void AlbumManager::showPicture()
{
	refreshOpenAlbum();

	const std::string picName = getInputFromConsole("Enter picture name: ");
	if (!m_openAlbum.doesPictureExists(picName))
		throw MyException("Error: There is no picture with name <" + picName + ">.\n");

	const auto pic = m_openAlbum.getPicture(picName);
	if (!fileExistsOnDisk(pic.getPath()))
		throw MyException("Error: Can't open <" + picName + "> since it doesn't exist on disk.\n");

	const PhotoViewApp app = getAppForUser();

	ImageAccess::openImageInApp(app, pic);
}

PhotoViewApp AlbumManager::getAppForUser()
{
	const auto menu_header =
		"\nPick a program to view the picture:\n"
		"-----------------------------------";
	const auto menu_items =
		"1. Paint (mspaint).\n"
		"2. IfranView.";


	std::cout << BLUE << menu_header << RESET << '\n';
	std::cout << MAGENTA << menu_items << RESET << '\n';
	std::string choice = getInputFromConsole("");

	while (choice != "1" && choice != "2")
	{
		std::cerr << RED << "Invalid choice. Please try again." << RESET << '\n';
		std::cout << BLUE << menu_header << RESET << '\n';
		std::cout << MAGENTA << menu_items << RESET << '\n';
		choice = getInputFromConsole("");
	}

	return choice == "1" ? PhotoViewApp::PAINT : PhotoViewApp::IRFANVIEW;
}

void AlbumManager::editPicturePermissions()
{
	refreshOpenAlbum();

	const std::string picName = getInputFromConsole("Enter picture name: ");
	if (!m_openAlbum.doesPictureExists(picName))
		throw MyException("Error: There is no picture with name <" + picName + ">.\n");

	const auto pic = m_openAlbum.getPicture(picName);
	if (!fileExistsOnDisk(pic.getPath()))
		throw MyException("Error: Can't open <" + picName + "> since it doesn't exist on disk.\n");

	const Permissions permissions = getWantedPermissionsFromUser();

	ImageAccess::setPermissions(pic, permissions);
}

Permissions AlbumManager::getWantedPermissionsFromUser()
{
	const auto menu_header =
		"\nWhat do you want to do:\n"
		"-----------------------------------";
	const auto menu_items =
		"1. Make picture Read-Only.\n"
		"2. Make picture Writeable.";


	std::cout << BLUE << menu_header << RESET << '\n';
	std::cout << MAGENTA << menu_items << RESET << '\n';
	std::string choice = getInputFromConsole("");

	while (choice != "1" && choice != "2")
	{
		std::cerr << RED << "Invalid choice. Please try again." << RESET << '\n';
		std::cout << BLUE << menu_header << RESET << '\n';
		std::cout << MAGENTA << menu_items << RESET << '\n';
		choice = getInputFromConsole("");
	}

	return choice == "1" ? Permissions::READ_ONLY : Permissions::WRITEABLE;
}

void AlbumManager::copyPicture()
{
	refreshOpenAlbum();

	const std::string picName = getInputFromConsole("Enter picture name: ");
	if (!m_openAlbum.doesPictureExists(picName))
		throw MyException("Error: There is no picture with name <" + picName + ">.\n");

	const auto pic = m_openAlbum.getPicture(picName);
	if (!fileExistsOnDisk(pic.getPath()))
		throw MyException("Error: Can't open <" + picName + "> since it doesn't exist on disk.\n");

	const std::string destPath = ImageAccess::availableCopyPictureName(pic);
	const std::string copiedPictureName = "CopyOf_" + picName;

	if (m_openAlbum.doesPictureExists(copiedPictureName))
		throw MyException("Error: Failed to copy picture, copied picture with the same name already exists.\n");

	ImageAccess::copyPicture(pic, destPath);

	Picture newPicture(++m_nextPictureId, copiedPictureName);
	newPicture.setPath(destPath);

	m_dataAccess.addPictureToAlbumByName(m_openAlbum.getName(), newPicture);
}

void AlbumManager::tagUserInPicture()
{
	refreshOpenAlbum();

	const std::string picName = getInputFromConsole("Enter picture name: ");
	if (!m_openAlbum.doesPictureExists(picName))
		throw MyException("Error: There is no picture with name <" + picName + ">.\n");


	const Picture pic = m_openAlbum.getPicture(picName);

	const std::string userIdStr = getInputFromConsole("Enter user id to tag: ");
	const int userId = std::stoi(userIdStr);

	if (!m_dataAccess.doesUserExists(userId))
		throw MyException("Error: There is no user with id @" + userIdStr + "\n");

	const User user = m_dataAccess.getUser(userId);

	m_dataAccess.tagUserInPicture(m_openAlbum.getName(), pic.getName(), user.getId());
	std::cout << GREEN << "User @" << userIdStr << " successfully tagged in picture <" << pic.getName() << "> in album [" << m_openAlbum.getName() << "]\n" << RESET;
}

void AlbumManager::untagUserInPicture()
{
	refreshOpenAlbum();

	const std::string picName = getInputFromConsole("Enter picture name: ");
	if (!m_openAlbum.doesPictureExists(picName))
		throw MyException("Error: There is no picture with name <" + picName + ">.\n");


	const Picture pic = m_openAlbum.getPicture(picName);

	const std::string userIdStr = getInputFromConsole("Enter user id: ");
	const int userId = stoi(userIdStr);

	if (!m_dataAccess.doesUserExists(userId))
		throw MyException("Error: There is no user with id @" + userIdStr + "\n");

	const User user = m_dataAccess.getUser(userId);

	if (!pic.isUserTagged(user))
		throw MyException("Error: The user was not tagged! \n");


	m_dataAccess.untagUserInPicture(m_openAlbum.getName(), pic.getName(), user.getId());
	std::cout << GREEN << "User @" << userIdStr << " successfully untagged in picture <" << pic.getName() << "> in album [" << m_openAlbum.getName() << "]\n" << RESET;
}

void AlbumManager::listUserTags()
{
	refreshOpenAlbum();

	const std::string picName = getInputFromConsole("Enter picture name: ");
	if (!m_openAlbum.doesPictureExists(picName))
		throw MyException("Error: There is no picture with name <" + picName + ">.\n");
	
	const auto pic = m_openAlbum.getPicture(picName);

	const std::set<int>& users = pic.getUserTags();

	if (users.empty())
		throw MyException("Error: There is no user tagged in <" + picName + ">.\n");

	std::cout << GREEN << "Tagged users in picture <" << picName << ">:\n" << RESET;
	for (const int user_id : users) {
		const User user = m_dataAccess.getUser(user_id);
		std::cout << GREEN << user << RESET << '\n';
	}
	std::cout << '\n';

}


// ******************* User ******************* 
void AlbumManager::addUser()
{
	const std::string name = getInputFromConsole("Enter user name: ");

	User user(++m_nextUserId, name);

	m_dataAccess.createUser(user);
	std::cout << GREEN << "User " << name << " with id @" << user.getId() << " created successfully.\n" << RESET;
}


void AlbumManager::removeUser()
{
	// get username
	const std::string userIdStr = getInputFromConsole("Enter user id: ");
	const int userId = std::stoi(userIdStr);

	if (!m_dataAccess.doesUserExists(userId))
		throw MyException("Error: There is no user with id @" + userIdStr + "\n");

	const User& user = m_dataAccess.getUser(userId);
	if (isCurrentAlbumSet() && userId == m_openAlbum.getOwnerId())
		closeAlbum();


	m_dataAccess.deleteUser(user);
	std::cout << GREEN << "User @" << userId << " deleted successfully." << '\n' << RESET;
}

void AlbumManager::listUsers()
{
	m_dataAccess.printUsers();
}

void AlbumManager::userStatistics()
{
	const std::string userIdStr = getInputFromConsole("Enter user id: ");
	const int userId = std::stoi(userIdStr);

	if (!m_dataAccess.doesUserExists(userId))
		throw MyException("Error: There is no user with id @" + userIdStr + "\n");


	const User& user = m_dataAccess.getUser(userId);

	std::cout << GREEN << "user @" << userId << " Statistics:" << '\n' << "--------------------" << '\n' << BLUE << 
		"  + Count of Albums Tagged: " << m_dataAccess.countAlbumsTaggedOfUser(user) << '\n' <<
		"  + Count of Tags: " << m_dataAccess.countTagsOfUser(user) << '\n' <<
		"  + Average Tags per Album: " << m_dataAccess.averageTagsPerAlbumOfUser(user) << '\n' <<
		"  + Number of owned Albums: " << m_dataAccess.countAlbumsOwnedOfUser(user) << '\n';
}


// ******************* Queries ******************* 
void AlbumManager::topTaggedUser()
{
	const User& user = m_dataAccess.getTopTaggedUser();

	std::cout << GREEN << "The top tagged user is: " << user.getName() << '\n' << RESET;
}

void AlbumManager::topTaggedPicture()
{
	const Picture& picture = m_dataAccess.getTopTaggedPicture();

	std::cout << GREEN << "The top tagged picture is: " << picture.getName() << '\n' << RESET;
}

void AlbumManager::picturesTaggedUser()
{
	const std::string userIdStr = getInputFromConsole("Enter user id: ");
	const int userId = std::stoi(userIdStr);

	if (!m_dataAccess.doesUserExists(userId))
		throw MyException("Error: There is no user with id @" + userIdStr + "\n");


	const auto user = m_dataAccess.getUser(userId);

	const auto taggedPictures = m_dataAccess.getTaggedPicturesOfUser(user);
	if (taggedPictures.empty())
		throw MyException("Error: There is no picture tagged by user@" + userIdStr + "\n");

	std::cout << GREEN <<  "List of pictures that user@" << user.getId() << " tagged:\n" << RESET;
	for (const Picture& picture : taggedPictures)
		std::cout << GREEN << "   + " << picture << '\n' << RESET;
	
	std::cout << '\n';
}


// ******************* Help & exit ******************* 
void AlbumManager::exit()
{
	std::exit(EXIT_SUCCESS);
}

void AlbumManager::help()
{
	system("CLS");
	printHelp();
}

std::string AlbumManager::getInputFromConsole(const std::string& message)
{
	std::string input;
	do {
		std::cout << MAGENTA << message << BLUE;
		std::getline(std::cin, input);
	} while (input.empty());

	std::cout << RESET;
	return input;
}

bool AlbumManager::fileExistsOnDisk(const std::string& filename)
{
	struct stat buffer;
	return stat(filename.c_str(), &buffer) == 0;
}

void AlbumManager::refreshOpenAlbum()
{
	if (!isCurrentAlbumSet())
		throw AlbumNotOpenException();

	m_openAlbum = m_dataAccess.openAlbum(m_currentAlbumName);
}

bool AlbumManager::isCurrentAlbumSet() const
{
	return !m_currentAlbumName.empty();
}

const std::vector<CommandGroup> AlbumManager::m_prompts = {
	{
		"Supported Albums Operations:"
				 "\n----------------------------",
		{
			{ CREATE_ALBUM        , "Create album." },
			{ OPEN_ALBUM          , "Open album." },
			{ CLOSE_ALBUM         , "Close album." },
			{ DELETE_ALBUM        , "Delete album." },
			{ LIST_ALBUMS         , "List albums." },
			{ LIST_ALBUMS_OF_USER , "List albums of user." }
		}
	},
	{
		"Supported Album commands (when specific album is open):"
		         "\n-------------------------------------------------------",
		{
			{ ADD_PICTURE    , "Add picture." },
			{ REMOVE_PICTURE , "Remove picture." },
			{ SHOW_PICTURE   , "Show picture (WinApi)." },
			{ LIST_PICTURES  , "List pictures." },
			{ TAG_USER		 , "Tag user." },
			{ UNTAG_USER	 , "Untag user." },
			{ LIST_TAGS		 , "List tags." }
		}
	},
	{
		"Supported Users commands: "
				"\n-------------------------",
		{
			{ ADD_USER         , "Add user." },
			{ REMOVE_USER      , "Remove user." },
			{ LIST_OF_USER     , "List of users." },
			{ USER_STATISTICS  , "User statistics." },
		}
	},
	{
		"Supported Queries:"
			   "\n------------------",
		{
			{ TOP_TAGGED_USER      , "Top tagged user." },
			{ TOP_TAGGED_PICTURE   , "Top tagged picture." },
			{ PICTURES_TAGGED_USER , "Pictures tagged user." },
		}
	},
	{
		"BONUS Operations:"
			   "\n-----------------",
			{
				{EDIT_PICTURE_PERMISSIONS, "Edit picture permissions."},
				{ COPY_PICTURE, "Copy picture."},
			}
	},
	{
		"Supported Operations:"
			   "\n---------------------",
		{
			{ HELP , "Help (clean screen)." },
			{ EXIT , "Exit." },
		}
	}
};

const std::map<CommandType, AlbumManager::handler_func_t> AlbumManager::m_commands = {
	{ CREATE_ALBUM, &AlbumManager::createAlbum },
	{ OPEN_ALBUM, &AlbumManager::openAlbum },
	{ CLOSE_ALBUM, &AlbumManager::closeAlbum },
	{ DELETE_ALBUM, &AlbumManager::deleteAlbum },
	{ LIST_ALBUMS, &AlbumManager::listAlbums },
	{ LIST_ALBUMS_OF_USER, &AlbumManager::listAlbumsOfUser },
	{ ADD_PICTURE, &AlbumManager::addPictureToAlbum },
	{ REMOVE_PICTURE, &AlbumManager::removePictureFromAlbum },
	{ LIST_PICTURES, &AlbumManager::listPicturesInAlbum },
	{ SHOW_PICTURE, &AlbumManager::showPicture },
	{ TAG_USER, &AlbumManager::tagUserInPicture, },
	{ UNTAG_USER, &AlbumManager::untagUserInPicture },
	{ LIST_TAGS, &AlbumManager::listUserTags },
	{ ADD_USER, &AlbumManager::addUser },
	{ REMOVE_USER, &AlbumManager::removeUser },
	{ LIST_OF_USER, &AlbumManager::listUsers },
	{ USER_STATISTICS, &AlbumManager::userStatistics },
	{ TOP_TAGGED_USER, &AlbumManager::topTaggedUser },
	{ TOP_TAGGED_PICTURE, &AlbumManager::topTaggedPicture },
	{ PICTURES_TAGGED_USER, &AlbumManager::picturesTaggedUser },
	{ EDIT_PICTURE_PERMISSIONS, &AlbumManager::editPicturePermissions },
	{ COPY_PICTURE, &AlbumManager::copyPicture },
	{ HELP, &AlbumManager::help },
	{ EXIT, &AlbumManager::exit }
};
