#pragma once
#include "DatabaseAccess.h"

class DataAccessTest
{
public:
	void createTables();
	void addData();
	void changeData();
	void deleteData();

private:
	DatabaseAccess _dbAccess;
};