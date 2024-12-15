#pragma once
#include "User.h"
#include <set>
#include <string>
#include <iomanip>

class Picture
{
public:
	Picture(int id, std::string name);
	Picture(int id, std::string name, std::string pathOnDisk, std::string creationDate);

	int getId() const;
	void setId(int id);

	const std::string& getName() const;
	void setName(const std::string& name);

	const std::string& getPath() const;
	void setPath(const std::string& location);

	const std::string& getCreationDate() const;
	void setCreationDate(const std::string& creationTime);
	void setCreationDateNow();

	bool isUserTagged(const User& user) const;
	bool isUserTagged(int userId) const;
	void tagUser(const User& user);
	void untagUser(const User& user);
	void untagUser(int userId);

	int getTagsCount() const;

	const std::set<User>& getUsersTagged() const;

	bool operator==(const Picture& other) const;
	friend std::ostream& operator<<(std::ostream& os, const Picture& pic);

private:
	int m_pictureId;
	std::string m_name;
	std::string m_pathOnDisk;
	std::string m_creationDate;
	std::set<User> m_usersTagged;
};
