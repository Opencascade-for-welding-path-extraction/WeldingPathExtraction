#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <opencascade/AIS_InteractiveContext.hxx>
#include "PartSolution.h"
#include <QDebug>
#include <Qstring>
#include "DisplayCore.h"
#include "CamGuiManager.h"
#include "CamWindowManager.h"

using CAM::MillPlanarGui;
using CAM::CamWindowManager;

class DockModelTree;
class WindownsManager;
class CAM::MillPlanarGui;
class CAM::CamWindowManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* par = nullptr);
    void InitWindow();
  
    
public:
    PartSolution* viewerWindow{ nullptr };//3D显示窗口
    CamWindowManager* myCamWindowManager;
    QTabWidget* MultiWindowsTabWidget{ nullptr };
    WindownsManager* windownsmanager{ nullptr };
    DockModelTree* dockmodeltree{nullptr};
    QDockWidget* dock{ nullptr };
    DockModelTree* dockmodeltreemap{ nullptr };
    DisplayCore*DisplayCoreManeger{ nullptr };
    QWidget* parent{ nullptr };
    QMenuBar* menubar;
    QStatusBar* statusbar;
    QAction* actionceshi, *actionImportPart,* actionGetLine;
    QMenu* menu;
    MillPlanarGui* mymillplanargui;
	int initializationsignal{0};//初始化界面完成信号

public slots:
    void myslot();
    void importPart();
    void getWeldingWire();

};
 
#endif  // MAINWINDOW_H
