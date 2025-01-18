#pragma once
#define _HAS_STD_BYTE 0  // before any includes
#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>
#include <QCheckBox>
#include <QMainWindow>
#include<string>
#include <QtWidgets>
#include "CamGuiManager.h"
#include "PartSolution.h"
#include "MillPlanar.h"

using CAM::PartSolution;
using namespace std;

class CAM::PartSolution;

namespace CAM
{

    class MillPlanar;
    class PageBuilder;
    class MillGui;
    class SelectCurveGui;

    class bottomBorderBarWidget :public QToolBar
    {
    public:
        bottomBorderBarWidget();
        void addPushbutton(QPushButton* button, Qt::Alignment alignment);
    public:
        QWidget* widget;
        QWidget* widgetCenter;
        QHBoxLayout* HBOX;
        QHBoxLayout* HBOXRight;
        QHBoxLayout* HBOXCenter;
        QHBoxLayout* HBOXLeft;

    };

    class operateBorderBarWidget :public QToolBar
    {
    public:
        operateBorderBarWidget();
        void addPushbutton(QPushButton* button, Qt::Alignment alignment);

    public:
        QWidget* widget;
        QWidget* widgetCenter;
        QHBoxLayout* HBOX;
        QHBoxLayout* HBOXRight;
        QHBoxLayout* HBOXCenter;
        QHBoxLayout* HBOXLeft;

    };
    class MillGui :public QMainWindow
    {
    public:
        MillGui();
        void CreatePage(const string pageFilePath, const string pageFileName);
        void SettingSort(QStringList& thekeys);
    public:
		QStackedWidget* stackedWidget;
        QDockWidget* dockWidget;
		map<string, QWidget*>LayoutWidgetMap;
		map<string, QPushButton*>pushButtonMap;
		map<string, QLabel*>LableMap;
		map<string, QComboBox*>ComboBoxMap;
		map<string, QPushButton*>PushButtonMap;
		map<string, QLineEdit*>LineEditMap;
		map<string, QCheckBox*>CheckBoxMap;
        map<string, PageBuilder*>pageBuilderMap;

    };
    class MillPlanarGui :public MillGui
    {
        Q_OBJECT
    public:
        MillPlanarGui(QSettings* par);
        void Init(PartSolution* par1, MillPlanar* par2);
        void SetupUi();
        void CreateTreeWidget();
        void InitialMainPageParameter();
        void SetQss();
        void BindingEvents();



    public:
        PartSolution* viewerWindow;
        QSettings* settings;
        MillPlanar *millPlanar;
        QWidget* widget;
        QTreeWidget* treeWidget;
        map<string, QWidget*>LayoutWidgetMap;
        map<string, QTreeWidgetItem*>widgetItemMap;
        map<string, bottomBorderBarWidget*>ToolBarMap;
        SelectCurveGui* selectCurveGui;
        QVBoxLayout* DockWidgetQVBoxLayout;
    public slots:
        void StackedWidgetPageChange();
        void SelectCuttingArea();
        void GetShapeSurface();
        void GetSelectMillContour();
        void SpecifyPart();
        void captureCurve();
        void captureFace();
        void GetSelsctPart();
        void CloseSettingWindow();
        void CreateWCS();
        void PushButtonSet();
        void SelectMillContour();
        void RemoveSelectedShape();

    };

    class WcsSettingGui :public MillGui
    {
        Q_OBJECT
    public:
        WcsSettingGui();
        ~WcsSettingGui() = default;
		void Init(PartSolution* par1, MillPlanar* par2,MillPlanarGui*par3);
		void SetupUi();
        void SetQss();

        
    public:
		PartSolution* viewerWindow;
		QSettings* settings;
		MillPlanar* millPlanar;
        MillPlanarGui* millPlanarGui;
		QWidget* widget;
		
		map<string, QWidget*>LayoutWidgetMap;
		map<string, QTreeWidgetItem*>widgetItemMap;
		map<string, bottomBorderBarWidget*>ToolBarMap;
		QVBoxLayout* DockWidgetQVBoxLayout;
    public:
        Standard_Integer ReverseX = 1;
        Standard_Integer ReverseY = 1;
	public slots:
		void StackedWidgetPageChange();
        void CloseWindow();
        void SpecifyPoint();
        void SpecifyVectorX();
        void SpecifyVectorY();
        void capturePoint();
        void captureShape();
        void GetOrigin();
        void GetVectorX();
        void GetVectorY();
        void ReverseVectorX();
        void ReverseVectorY();




    };
    class SelectCurveGui :public MillGui
    {
        Q_OBJECT
    public:
        SelectCurveGui();
        ~SelectCurveGui()=default;
		void Init(PartSolution* par1, MillPlanar* par2, MillPlanarGui* par3);
		void SetupUi();
		void SetQss();

    public:
		PartSolution* viewerWindow;
		QSettings* settings;
		MillPlanar* millPlanar;
		MillPlanarGui* millPlanarGui;
		QWidget* widget;
		map<string, QWidget*>LayoutWidgetMap;
		map<string, QTreeWidgetItem*>widgetItemMap;
		map<string, bottomBorderBarWidget*>ToolBarMap;
		QVBoxLayout* DockWidgetQVBoxLayout;
    public slots:
        void ShowSelectCurveNum();
        void CloseWindow();


       

    };

    class PageItem
    {
     /* "QComboBox", "QLabel", "QPushButton", "QSpacerItem", "QLineEdit", "QFrame"*/
    public:
        PageItem() = default;
        PageItem(string lable, string theItemKind);
        PageItem(string lable, string theItemKind,string theItemName);
    public:
        string lable;
        string itemKind;
        string itemName;
    public:
        void SetLable(string lable);
        void SetItemKind(string theItemKind);
        void SetItemName(string theItemName);

    };

    class PageBuilder:public QMainWindow
    {
       
    public:
        PageBuilder();
        void Init(MillGui* theparent);
        void Build();
        void AddPanel(string itemNme);
        void CreateItem();
        void AddItem(string panelName, list<PageItem> itemList);
        void SetPanel(string Oldpanel, string NewPanel);
        void Finished();
    public:
       list<PageItem>itemList;
	public:
        MillGui* parent;
		QWidget* page;
		QScrollArea* Scroll;
		QVBoxLayout* pageQVBoxLayout;
        map<string, QWidget*>LayoutWidgetMap;
		map<string, QHBoxLayout*>PageQHBoxLayoutMap;
        map<string, QSpacerItem*>SpacerItemMap;
        map<string, QFrame*>FramelineMap;
		QVBoxLayout* PagQVBoxLayout;

       
    };
}
