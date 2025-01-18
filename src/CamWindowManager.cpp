#include "CamWindowManager.h"
#include"DisplayCore.h"
#include<QFileDialog>
#include "MillPlanar.h"
#include "CamGuiManager.h"

using namespace CAM;

CAM::CamWindowManager::CamWindowManager(QWidget* par) 
{
	parent = par;
	InitialSettings();
	/*3D显示窗口*/
	viewerWindow = new PartSolution(parent); 
	/*加工参数GUI*/
	myMillPlanarGui = new MillPlanarGui(settings);
	/*加工模块*/
	millPlanar = new MillPlanar();
	/*初始化*/
	myMillPlanarGui->Init(viewerWindow, millPlanar);
	millPlanar->Init(myMillPlanarGui, viewerWindow);
	/*信号和槽绑定*/
	connect(myMillPlanarGui->pushButtonMap["generate"], SIGNAL(clicked()), this, SLOT(CreateToolPath()));



}


CAM::CamWindowManager::CamWindowManager()
{
	viewerWindow = new PartSolution(); //不指定父窗口，单独用show()方法显示
	
}

void CAM::CamWindowManager::InitialSettings()
{
	/*初始化setting*/
	QString fileName;
	fileName = QCoreApplication::applicationDirPath();
	fileName += "/config/data.ini";
	settings = new QSettings(fileName, QSettings::IniFormat);
	/*读取data.ini*/

}

void CAM::CamWindowManager::OpenPart()
{


}

void CAM::CamWindowManager::CreateToolPath()
{
	millPlanar->CreateToolPath();
}

void CAM::CamWindowManager::ImportPart()
{
}