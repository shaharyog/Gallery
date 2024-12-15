#include "ImageAccess.h"

#include <Windows.h>

#include "AlbumManager.h"
#include "Colors.h"
#include "MyException.h"

void ImageAccess::openImageInApp(const PhotoViewApp& app, const Picture& picture)
{
	const std::string appPath = getAppPath(app);

	const std::string& picPath = picture.getPath();
	if (!doesFileExistOnDisk(picPath))
		throw MyException("Picture '" + picture.getName() + "' in Location: <" + picPath + "> doesn't exist!");

	const std::string commandLine = appPath + " \"" + picPath + "\"";

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = true;

	const auto fileInfoBefore = FileInfo(picPath);

	const BOOL res = CreateProcess(NULL, const_cast<char*>(commandLine.c_str()), NULL, NULL, false, 0, NULL, NULL, &si, &pi);
	if (!res)
		throw MyException("Could not open image.\nError: " + std::to_string(GetLastError()));

	current_viewer_process = pi.hProcess;
	
	// Register Ctrl+C handler
	if (!SetConsoleCtrlHandler(CtrlCHandler, TRUE))
		throw MyException("Error registering Ctrl+C handler.");

	// wait for the process to finish
	WaitForSingleObject(pi.hProcess, INFINITE);	

	// Unregister Ctrl+C handler
	SetConsoleCtrlHandler(CtrlCHandler, FALSE);

	current_viewer_process = nullptr;

	const auto fileInfoAfter = FileInfo(picPath);

	printImageModifications(fileInfoBefore, fileInfoAfter);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void ImageAccess::setPermissions(const Picture& picture, const Permissions& permissions)
{
	const std::string& picPath = picture.getPath();

	if (!doesFileExistOnDisk(picPath))
		throw MyException("Picture '" + picture.getName() + "' in Location: <" + picPath + "> doesn't exist!");

	if (permissions == Permissions::READ_ONLY)
		makePictureReadOnly(picture);
	else
		makePictureWritable(picture);
}

std::string ImageAccess::availableCopyPictureName(const Picture& picture)
{
	const std::string& picPath = picture.getPath();

	if (!doesFileExistOnDisk(picPath))
		throw MyException("Picture '" + picture.getName() + "' in Location: <" + picPath + "> doesn't exist!");

	const std::filesystem::path originalPath(picPath);

	const std::string directoryPath = originalPath.parent_path().string();

	const std::string newFileName = "CopyOf_" + originalPath.stem().string() + originalPath.extension().string();

	std::string newFilePath = directoryPath + "\\" + newFileName;

	if (doesFileExistOnDisk(newFilePath))
		throw MyException("Cannot copy picture to '" + newFilePath + "' since it already exists!");

	return newFilePath;
}

void ImageAccess::copyPicture(const Picture& picture, const std::string& destPath)
{
	const std::string& picPath = picture.getPath();

	if (!doesFileExistOnDisk(picPath))
		throw MyException("Picture '" + picture.getName() + "' in Location: <" + picPath + "> doesn't exist!");

	std::filesystem::copy_file(picPath, destPath);
}

void ImageAccess::makePictureReadOnly(const Picture& picture)
{
	const std::string& picPath = picture.getPath();
	if (!doesFileExistOnDisk(picPath))
		throw MyException("Picture '" + picture.getName() + "' in Location: <" + picPath + "> doesn't exist!");

	permissions(picPath, std::filesystem::perms::owner_read | std::filesystem::perms::group_read | std::filesystem::perms::others_read);
}

void ImageAccess::makePictureWritable(const Picture& picture)
{
	const std::string& picPath = picture.getPath();
	if (!doesFileExistOnDisk(picPath))
		throw MyException("Picture '" + picture.getName() + "' in Location: <" + picPath + "> doesn't exist!");

	permissions(picPath, std::filesystem::perms::owner_write | std::filesystem::perms::group_write | std::filesystem::perms::others_write);
}

bool ImageAccess::doesFileExistOnDisk(const std::string& filename)
{
	return std::filesystem::exists(filename);
}

std::string ImageAccess::getAppPath(const PhotoViewApp& app)
{
	const int appIndex = static_cast<int>(app);
	if (!(appIndex >= 0 && appIndex < static_cast<int>(std::size(APP_PATHS))))
		throw MyException("Invalid app!");

	const std::string appPath = APP_PATHS[appIndex];

	if (appIndex == 0)
		return appPath; // mspaint already in path

	if (!doesFileExistOnDisk(appPath))
		throw MyException("Photo Viewer App in Location: <" + appPath + "> doesn't exist!");

	return appPath;
}


BOOL WINAPI ImageAccess::CtrlCHandler(DWORD ctrlType)
{
	if (ctrlType == CTRL_C_EVENT) {
		std::cout << BLUE << "Ctrl+C pressed. Terminating viewer process..." << RESET << '\n';

		if (current_viewer_process != nullptr)
			TerminateProcess(current_viewer_process, 0);

		return TRUE;
	}
	return FALSE;
}

void ImageAccess::printImageModifications(const FileInfo& fileInfoBefore, const FileInfo& fileInfoAfter)
{
	if (fileInfoBefore.lastWriteTime == fileInfoAfter.lastWriteTime)
	{
		std::cout << GREEN << "File was not modified." << RESET << '\n';
		return;
	}

	if (fileInfoBefore.fileSize == fileInfoAfter.fileSize)
	{
		std::cout << GREEN << "The file was modified but the size remained the same." << RESET << '\n';
		return;
	}
	
	const auto size_before_viewer = fileInfoBefore.fileSize / 1024;
	const auto size_after_viewer = fileInfoAfter.fileSize / 1024;

	if (size_before_viewer < size_after_viewer)
		std::cout << GREEN << "The file was modified and the size has been increased from " << size_before_viewer << "KB to " << size_after_viewer << "KB." << RESET << '\n';
	else
		std::cout << GREEN << "The file was modified and the size has been dropped from " << size_before_viewer << "KB to " << size_after_viewer << "KB." << RESET << '\n';
	
	

}


