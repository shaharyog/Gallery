#pragma once
#include <filesystem>
#include <Windows.h>

#include "Constants.h"
#include "Picture.h"


struct FileInfo
{
	std::filesystem::path path;
	std::filesystem::file_time_type lastWriteTime;
	uintmax_t fileSize;
	std::filesystem::perms permissions;

	FileInfo(const std::string& path) : path(path)
	{
		lastWriteTime = std::filesystem::last_write_time(path);
		fileSize = std::filesystem::file_size(path);
		permissions = std::filesystem::status(path).permissions();
	}
};


class ImageAccess
{
public:
	static void openImageInApp(const PhotoViewApp& app, const Picture& picture);
	static void setPermissions(const Picture& picture, const Permissions& permissions);
	static std::string availableCopyPictureName(const Picture& picture);
	static void copyPicture(const Picture& picture, const std::string& destPath);

private:
	static bool doesFileExistOnDisk(const std::string& filename);
	static std::string getAppPath(const PhotoViewApp& app);
	static BOOL WINAPI CtrlCHandler(DWORD ctrlType);

	static void printImageModifications(const FileInfo& fileInfoBefore, const FileInfo& fileInfoAfter);

	static void makePictureReadOnly(const Picture& picture);
	static void makePictureWritable(const Picture& picture);

	inline static HANDLE current_viewer_process = nullptr;
};
