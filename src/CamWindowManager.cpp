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
	/*3D��ʾ����*/
	viewerWindow = new PartSolution(parent); 
	/*�ӹ�����GUI*/
	myMillPlanarGui = new MillPlanarGui(settings);
	/*�ӹ�ģ��*/
	millPlanar = new MillPlanar();
	/*��ʼ��*/
	myMillPlanarGui->Init(viewerWindow, millPlanar);
	millPlanar->Init(myMillPlanarGui, viewerWindow);
	/*�źźͲ۰�*/
	connect(myMillPlanarGui->pushButtonMap["generate"], SIGNAL(clicked()), this, SLOT(CreateToolPath()));



}


CAM::CamWindowManager::CamWindowManager()
{
	viewerWindow = new PartSolution(); //��ָ�������ڣ�������show()������ʾ
	
}

void CAM::CamWindowManager::InitialSettings()
{
	/*��ʼ��setting*/
	QString fileName;
	fileName = QCoreApplication::applicationDirPath();
	fileName += "/config/data.ini";
	settings = new QSettings(fileName, QSettings::IniFormat);
	/*��ȡdata.ini*/

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