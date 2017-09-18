// Khexplorer.cpp : Defines the entry point for the console application.
//

#include <iostream>

#include "SmartKhepera.h"

void ListCommands();

static const std::string s_StartRunningCmd = "go";
static const std::string s_StopRunningCmd = "stop";
static const std::string s_StartVSCmd = "smart";
static const std::string s_StopVSCmd = "stupid";
static const std::string s_StartInfoCmd = "babble";
static const std::string s_StopInfoCmd = "quiet";
static const std::string s_Help = "help";
static const std::string s_Settings = "settings";
static const std::string s_EndProgram = "exit";

static const std::string s_StopOperator = "hobble";

static const std::string s_SaveRBFNodesCmd = "save";
static const std::string s_LoadRBFNodesCmd = "load";
static const std::string path = "Controller.txt";

int main()
{
	CSmartKhepera* pKhexplore = nullptr;
/*
	while(pKhexplore == nullptr)
	{
		std::cout << "Accessing Khepera... ";
		try
		{
			pKhexplore = new CSmartKhepera();
		}
		catch(...)
		{
			std::cout << "Failed!" << std::endl;
			delete pKhexplore;
			pKhexplore = nullptr;
		}
	}
*/
	pKhexplore = new CSmartKhepera();

	// print welcome and help
	std::cout << "Welcome to the Khexplorer. ";
	ListCommands();

	// run and listen for instructions until exit
	std::string command;
	do
	{
		std::cout << "What would you like to do? ";
		std::cin >> command;

		if (command == s_StartRunningCmd) pKhexplore->StartRobot();
		if (command == s_StopRunningCmd) pKhexplore->StopRobot();
		if (command == s_StartVSCmd) pKhexplore->StartLearning();
		if (command == s_StopVSCmd) pKhexplore->StopLearning();
		if (command == s_StartInfoCmd) pKhexplore->StartVerbosity();
		if (command == s_StopInfoCmd) pKhexplore->StopVerbosity();
		if (command == s_StopOperator) pKhexplore->StopMoving();
		if (command == s_Settings)
		{
			pKhexplore->OpenSettingsMenu();
			ListCommands();
		}

		if (command == s_SaveRBFNodesCmd) pKhexplore->SaveNodes(path);
		if (command == s_LoadRBFNodesCmd) pKhexplore->LoadNodes(path);

		if (command == s_Help) ListCommands();
	} while (command != s_EndProgram);

	// stop our robot!
	pKhexplore->StopLearning();
	pKhexplore->StopRobot();
	delete pKhexplore;

	std::cout << "Goodbye!" << std::endl;
    return 0;
}

void ListCommands()
{
	std::cout << "To issue a command, please type into the console and press \'enter\'." << std::endl;
	std::cout << "Possible commands: " << std::endl;
	std::cout << "   " << s_Help << "         : lists this help" << std::endl;
	std::cout << "   " << s_EndProgram << "         : ends the program" << std::endl;
	std::cout << "   " << s_Settings << "     : opens the settings menu" << std::endl;
	std::cout << "   " << s_StartRunningCmd << "           : starts the robot" << std::endl;
	std::cout << "   " << s_StopRunningCmd << "         : stops the robot" << std::endl;
	std::cout << "   " << s_StartVSCmd << "        : enables robot learning" << std::endl;
	std::cout << "   " << s_StopVSCmd << "       : disables robot learning" << std::endl;
	std::cout << "   " << s_StartInfoCmd << "       : enables info dumping" << std::endl;
	std::cout << "   " << s_StopInfoCmd << "        : disables info dumping" << std::endl;
    
	std::cout << "   " << s_SaveRBFNodesCmd << "    : save the RBF nodes to file" << std::endl;
	std::cout << "   " << s_LoadRBFNodesCmd << "    : load the RBF nodes from file" << std::endl;

	std::cout << "   " << s_StopOperator << "       : disables setting speeds" << std::endl;
    
	std::cout << std::endl;
}
