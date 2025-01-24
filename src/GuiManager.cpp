#include "GuiManager.h"
#include <QApplication>
#include <QFile>
#include <QTextEdit>
#include <QAbstractButton>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QStatusBar>
#include <QDebug>
#include <QElapsedTimer>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCalendarWidget>
#include <QXmlStreamWriter>
#include <QTextStream>
#include <QFontComboBox>
#include <QLabel>
#include "occView.h"
#include <chrono>
#include <iostream>
#include"DockModelTree.h"
#include<QScreen>
#include<map>
#include "PartSolution.h"
#include <QDebug>
#include"WindownsManager.h"
#include <QThread>
#include"CamWindowManager.h"
#include"CamGuiManager.h"
using namespace CAM;

#define PRINT_COST_START()                                                                                             \
    QElapsedTimer __TMP_COST;                                                                                          \
    __TMP_COST.start();                                                                                                \
    int __TMP_LASTTIMES = 0

#define PRINT_COST(STR)                                                                                                \
    do {                                                                                                               \
        int ___TMP_INT = __TMP_COST.elapsed();                                                                         \
        qDebug() << STR << " cost " << ___TMP_INT - __TMP_LASTTIMES << " ms (" << ___TMP_INT << ")";                   \
        m_edit->append(QString("%1 cost %2 ms(%3)").arg(STR).arg(___TMP_INT - __TMP_LASTTIMES).arg(___TMP_INT));       \
        __TMP_LASTTIMES = ___TMP_INT;                                                                                  \
    } while (0)


/*使用案例*/
MainWindow::MainWindow(QWidget* par) 
{
    PRINT_COST_START();
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

