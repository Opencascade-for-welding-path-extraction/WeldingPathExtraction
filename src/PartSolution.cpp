#include "PartSolution.h"
#include"OccView.h"
#include<QtCore/Qstring>
#include<QFileDialog>
#include"DisplayCore.h"
#include<opencascade/Graphic3d_GraphicDriver.hxx>
#include "InteractiveModule.h"
using namespace std;
using namespace CAM;

class CAM::OccView;
class CAM::DisplayCore;

static Handle(Graphic3d_GraphicDriver)& GetGraphicDriver()
{
	static Handle(Graphic3d_GraphicDriver) aGraphicDriver;
	return aGraphicDriver;
}

CAM::PartSolution::PartSolution(QWidget *parent): QMainWindow(parent)
{
	myOccView = new OccView(parent);
	DisplayCoreManager = new DisplayCore(myOccView);
	interactivemanager = new InteractiveManager(DisplayCoreManager);
	myOccView->reset();


	
}

CAM::PartSolution::~PartSolution()
{
}

void CAM::PartSolution::OpenPart()
{
	
}

void CAM::PartSolution::ImportPart()
{
	QString filePath = QFileDialog::getOpenFileName(
		parent,
		QString::fromLocal8Bit("选择打开文件"),
		"c:/",
		tr("step(*step *stp )"));
    if (filePath.size()!=0)
    {
		QStringList fileNameList = filePath.split("/");
		QString fileName = fileNameList[fileNameList.size() - 1];
		fileNameList = fileName.split(".");
		fileName = fileNameList[0];
		DisplayCoreManager->Read_step_file(filePath.toStdString(), fileName.toStdString());
    }
    	
	

}
