#include <chrono>
#include <iostream>
#include <string>
#include "AlbumManager.h"
#include "Colors.h"
#include "DatabaseAccess.h"


int getCommandNumberFromUser()
{
	const std::string message("\nPlease enter any command number: ");
	const std::string numericStr("0123456789");

	std::cout << CYAN << message << '\n';
	std::string input;
	std::getline(std::cin, input);

	while (std::cin.fail() || std::cin.eof() || input.find_first_not_of(numericStr) != std::string::npos || input.empty())
	{

		std::cout << RED << "Please enter a number only!" << '\n';

		if (input.find_first_not_of(numericStr) == std::string::npos)
			std::cin.clear();

		std::cout << CYAN << '\n' << message << '\n';
		std::getline(std::cin, input);
	}
	std::cout << RESET;

	return std::stoi(input);
}

void PrintSystemInfo()
{
	std::cout << MAGENTA << "Developer: Shahar Yogev" << RESET << '\n';

	// get the time
	const auto now = std::chrono::system_clock::now();
	const std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
	const std::tm* now_tm = std::localtime(&now_time_t);

	std::cout << MAGENTA << "Current Date: " << std::setfill('0') << std::setw(2) << now_tm->tm_mday << "/"
		<< std::setfill('0') << std::setw(2) << now_tm->tm_mon + 1 << "/"
		<< now_tm->tm_year + 1900 << RESET << '\n';

	std::cout << MAGENTA << "Current Time: " << std::setfill('0') << std::setw(2) << now_tm->tm_hour << ":"
		<< std::setfill('0') << std::setw(2) << now_tm->tm_min << ":"
		<< std::setfill('0') << std::setw(2) << now_tm->tm_sec << RESET << "\n\n";

	std::cout << std::setfill(' ');	// reset
}

int main()
{
	// initialization data access
	DatabaseAccess dataAccess;

	// initialize album manager
	AlbumManager albumManager(dataAccess);


	std::string albumName;
	std::cout << YELLOW << "Welcome to Gallery!" << RESET << '\n';
	std::cout << YELLOW << "===================" << RESET << '\n';

	PrintSystemInfo();


	std::cout << BLUE << "Type " << HELP << " to a list of all supported commands" << RESET << '\n';

	do {
		int commandNumber = getCommandNumberFromUser();

		try {
			albumManager.executeCommand(static_cast<CommandType>(commandNumber));
		}
		catch (std::exception& e) {
			std::cout << RED << e.what() << RESET << '\n';
		}
	} while (true);
}


