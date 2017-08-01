#include <iostream>

#include "SmartKhepera.h"

static const std::string s_MaxSpeed = "speed";
static const std::string s_RbfNodeCount = "nodes";
static const std::string s_RbfSigma = "sigma";
static const std::string s_RbfLearnFactor = "learn";

static const std::string s_Help = "help";
static const std::string s_EndProgram = "exit";

void ListSettings();

CSmartKhepera::CSmartKhepera()
{
	m_pRbfSettings = new CRbfSettings();

	m_pUtil = new CKheperaUtility();
	m_pControl = new CController(m_pUtil, m_pRbfSettings);
	m_pOperate = new COperator(m_pUtil);
	m_pValues = new CValueSystem(m_pUtil);
}

CSmartKhepera::~CSmartKhepera()
{
	StopLearning();
	StopRobot();
	
	delete m_pValues;
	delete m_pOperate;
	delete m_pControl;
	delete m_pUtil;

	delete m_pRbfSettings;
}


void CSmartKhepera::StartRobot()
{
	m_pControl->Start();
	m_pOperate->Start();
}

void CSmartKhepera::StopRobot()
{
    StopLearning();
	m_pOperate->Stop();
	m_pControl->Stop();
	m_pUtil->SetSpeed(0, 0);
    
    m_pControl->ListNodes();
}

void CSmartKhepera::StartLearning()
{
	m_pValues->Start();
}

void CSmartKhepera::StopLearning()
{
	m_pValues->Stop();
}

void CSmartKhepera::StopMoving()
{
    m_pOperate->Stop();
    m_pUtil->SetSpeed(0, 0);
}

void CSmartKhepera::StartVerbosity()
{
	m_pUtil->SetVerbosity(true);
}

void CSmartKhepera::StopVerbosity()
{
	m_pUtil->SetVerbosity(false);
}

void CSmartKhepera::OpenSettingsMenu()
{
	ListSettings();

	std::string command;
	double val;
	do
	{
		std::cout << "What would you like to set? ";
		std::cin >> command;

		if (command != s_Help && command != s_EndProgram)		std::cin >> val;

		if (command == s_MaxSpeed) m_pUtil->MaxSpeed = val;
		if (command == s_RbfNodeCount) m_pRbfSettings->MaxNodes = round(val);
		if (command == s_RbfSigma) CNode::Sigma = val;
		if (command == s_RbfLearnFactor) CNode::LearningWeight = val;
		if (command == s_Help) ListSettings();
	} while (command != s_EndProgram);

}

void ListSettings()
{
	std::cout << "To enter a new setting, type its name followed by the new value into the console and press \'enter\'." << std::endl;
	std::cout << "Possible settings: " << std::endl;
	std::cout << "   " << s_Help << "         : lists this help" << std::endl;
	std::cout << "   " << s_EndProgram << "         : exits the settings" << std::endl;

	std::cout << "   " << s_MaxSpeed << "        : maximum robot speed" << std::endl;
	std::cout << "   " << s_RbfNodeCount << "        : RBF network maximum node count" << std::endl;
	std::cout << "   " << s_RbfSigma << "        : RBF network node width" << std::endl;
	std::cout << "   " << s_RbfLearnFactor << "        : RBF network node learning weight" << std::endl;

	std::cout << std::endl;
}


void CSmartKhepera::SaveNodes(std::string path)
{
	m_pControl->SaveNodesToFile(path);

}
void CSmartKhepera::LoadNodes(std::string path)
{
	m_pControl->LoadNodesFromFile(path);

}