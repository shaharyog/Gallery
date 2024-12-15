#pragma once
#include <map>
#include <vector>
#include "Constants.h"
#include "MemoryAccess.h"
#include "Album.h"


class AlbumManager
{
public:
	AlbumManager(IDataAccess& dataAccess);

	void executeCommand(CommandType command);
	static void printHelp();

	using handler_func_t = void (AlbumManager::*)(void);

private:
	int m_nextPictureId{};
	int m_nextUserId{};
	std::string m_currentAlbumName{};
	IDataAccess& m_dataAccess;
	Album m_openAlbum;

	void help();
	// albums management
	void createAlbum();
	void openAlbum();
	void closeAlbum();
	void deleteAlbum();
	void listAlbums();
	void listAlbumsOfUser();

	// Picture management
	void addPictureToAlbum();
	void removePictureFromAlbum();
	void listPicturesInAlbum();
	void showPicture();
	static PhotoViewApp getAppForUser();
	void editPicturePermissions();
	static Permissions getWantedPermissionsFromUser();
	void copyPicture();

	// tags related
	void tagUserInPicture();
	void untagUserInPicture();
	void listUserTags();

	// users management
	void addUser();
	void removeUser();
	void listUsers();
	void userStatistics();

	void topTaggedUser();
	void topTaggedPicture();
	void picturesTaggedUser();
	void exit();

	static std::string getInputFromConsole(const std::string& message);
	static bool fileExistsOnDisk(const std::string& filename);
	void refreshOpenAlbum();
	bool isCurrentAlbumSet() const;

	static const std::vector<CommandGroup> m_prompts;
	static const std::map<CommandType, handler_func_t> m_commands;

};

