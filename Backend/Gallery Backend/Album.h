#pragma once
#include "Picture.h"
#include <list>


class Album
{
public:
	Album() = default;
	Album(int ownerId, std::string name);
	Album(int ownerId, std::string name, std::string creationTime);

	const std::string& getName() const;
	void setName(const std::string& name);

	int getOwnerId() const;
	void setOwner(int userId);

	std::string getCreationDate() const;
	void setCreationDate(const std::string& creationTime);
	void setCreationDateNow();

	std::string getOwnerName() const;
	void setOwnerName(const std::string& name);

	bool doesPictureExists(const std::string& name) const;
	void addPicture(const Picture& picture);
	void removePicture(const std::string& pictureName);

	Picture getPicture(const std::string& name) const;
	std::list<Picture> getPictures() const;

	void untagUserInAlbum(int userId);
	void tagUserInAlbum(const User& userId);

	void untagUserInPicture(int userId, const std::string& pictureName);
	void tagUserInPicture(const User& user, const std::string& pictureName);

	bool operator==(const Album& other) const;
	friend std::ostream& operator<<(std::ostream& strOut, const Album& album);

private:
	int m_ownerId{ 0 };
	std::string m_owner_name;
	std::string m_name;
	std::string m_creationDate;
	std::list<Picture> m_pictures;
};
