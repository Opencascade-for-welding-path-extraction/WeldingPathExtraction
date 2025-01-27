#include "occView.h"
#include "PartSolution.h"
#include"CamWindowManager.h"
#include"CamGuiManager.h"
#include "GuiManager.h"
using namespace CAM;



/*使用案例*/
MainWindow::MainWindow(QWidget* par) 
{
    parent = par;
    setWindowTitle(("CAM"));
    myCamWindowManager = new CamWindowManager(this);
    actionceshi = new QAction(this);
    actionceshi->setObjectName(QString::fromUtf8("actionceshi"));

	actionImportPart = new QAction(this);
    actionImportPart->setObjectName(QString::fromUtf8("actionImportPart"));

	actionGetLine = new QAction(this);
    actionGetLine->setObjectName(QString::fromUtf8("actionGetLine"));

    setStatusBar(new QStatusBar());
    setCentralWidget(myCamWindowManager->viewerWindow->myOccView);
    menubar = new QMenuBar(this);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 800, 23));
    setMenuBar(menubar);
    menu = menuBar()->addMenu(QString::fromLocal8Bit("加工模块"));
    menu->addAction(actionceshi);
    actionceshi->setText(QString::fromLocal8Bit("平面加工"));

	menu->addAction(actionImportPart);
    actionImportPart->setText(QString::fromLocal8Bit("导入零件"));

	menu->addAction(actionGetLine);
    actionGetLine->setText(QString::fromLocal8Bit("提取焊接线"));

    //mymillplanargui = new MillPlanarGui(myCamWindowManager->viewerWindow->myOccView);

    connect(actionceshi, SIGNAL(triggered()), this, SLOT(myslot()));
    connect(actionImportPart, SIGNAL(triggered()), this, SLOT(importPart()));
    connect(actionGetLine, SIGNAL(triggered()), this, SLOT(getWeldingWire()));
  
   

    

}

void MainWindow::InitWindow()
{
   
}
void MainWindow::importPart()
{
    myCamWindowManager->viewerWindow->ImportPart();
}
void MainWindow::getWeldingWire()
{
	
	myCamWindowManager->millPlanar->GetWeldingWire();
}
void MainWindow::myslot()
{
    myCamWindowManager-> myMillPlanarGui->show();
}

