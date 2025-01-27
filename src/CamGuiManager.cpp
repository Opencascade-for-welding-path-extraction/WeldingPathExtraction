#include "CamGuiManager.h"
#include <QDockWidget>
#include <QToolBar>
#include <QtWidgets>
#include<string>
#include <QHBoxLayout>
#include <QSpacerItem >
#include<QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QStringList>
#include <QVariant>
#include "InteractiveModule.h"
#include "CamWindowManager.h"

using namespace std;

CAM::MillPlanarGui::MillPlanarGui(QSettings* par)
{
    settings = par;
    SetupUi();
    /*按钮信号和槽绑定*/
    connect(pushButtonMap["ok"], SIGNAL(clicked()), this, SLOT(CloseSettingWindow()));//确定关闭
	connect(pushButtonMap["cancel"], SIGNAL(clicked()), this, SLOT(CloseSettingWindow()));//取消关闭
    connect(pushButtonMap["坐标系-新建"], SIGNAL(clicked()), this, SLOT(CreateWCS()));//显示建立坐标系的界面
	connect(pushButtonMap["指定部件-指定"], SIGNAL(clicked()), this, SLOT(SpecifyPart()));//指定加工零件
    connect(pushButtonMap["指定切削区域底面-指定"], SIGNAL(clicked()), this, SLOT(SelectCuttingArea()));//指定切削区域底面


}


void CAM::MillPlanarGui::Init(PartSolution* par1, MillPlanar* par2)
{
	viewerWindow = par1;
	millPlanar = par2;
}

void CAM::MillPlanarGui::SetupUi()
{   
    dockWidget = new QDockWidget(tr("DockWidget"), this);
    dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dockWidget->setMinimumWidth(200);
    dockWidget->setGeometry(0, 0,50,300);
    this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

    treeWidget = new QTreeWidget(dockWidget);
    treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
    treeWidget->setGeometry(QRect(0, 0, 211, 441));
    treeWidget->header()->setVisible(false);
    treeWidget->setStyle(QStyleFactory::create("windows"));

    stackedWidget = new QStackedWidget(this);
    stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
    stackedWidget->setGeometry(QRect(150, 10, 331, 421));
    stackedWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(240, 240, 240);"));
    ToolBarMap["bottomtoolbar"] = new bottomBorderBarWidget();
    addToolBar(Qt::BottomToolBarArea, ToolBarMap["bottomtoolbar"]);
    ToolBarMap["toptoolbar"] = new bottomBorderBarWidget();
    addToolBar(Qt::TopToolBarArea, ToolBarMap["toptoolbar"]);

    operateBorderBarWidget* operateBorderBar = new operateBorderBarWidget();
    addToolBar(Qt::BottomToolBarArea, operateBorderBar);
    insertToolBarBreak(operateBorderBar);


    pushButtonMap["ok"] = new QPushButton();
    pushButtonMap["ok"]->setGeometry(QRect(0, 0, 90, 30));
    pushButtonMap["ok"]->setText(QString::fromLocal8Bit("确定"));

    pushButtonMap["cancel"] = new QPushButton();
    pushButtonMap["cancel"]->setGeometry(QRect(0, 0, 90, 30));
    pushButtonMap["cancel"]->setText(QString::fromLocal8Bit("取消"));

	pushButtonMap["generate"] = new QPushButton();
	pushButtonMap["generate"]->setGeometry(QRect(0, 0, 90, 90));
	pushButtonMap["generate"]->setText(QString::fromLocal8Bit("生成"));

	pushButtonMap["redisplay"] = new QPushButton();
	pushButtonMap["redisplay"]->setGeometry(QRect(0, 0, 90, 90));
	pushButtonMap["redisplay"]->setText(QString::fromLocal8Bit("重播"));

	pushButtonMap["confirm"] = new QPushButton();
	pushButtonMap["confirm"]->setGeometry(QRect(0, 0, 90, 30));
	pushButtonMap["confirm"]->setText(QString::fromLocal8Bit("确认"));

    operateBorderBar->addPushbutton(pushButtonMap["generate"], Qt::AlignLeft);
    operateBorderBar->addPushbutton(pushButtonMap["redisplay"], Qt::AlignLeft);
    operateBorderBar->addPushbutton(pushButtonMap["confirm"], Qt::AlignLeft);



    ToolBarMap["bottomtoolbar"]->addPushbutton(pushButtonMap["ok"],Qt::AlignLeft);
    ToolBarMap["bottomtoolbar"]->addPushbutton(pushButtonMap["cancel"], Qt::AlignRight);

   
    setCentralWidget(stackedWidget);
    resize(600, 750);
    CreateTreeWidget();
    CreatePage("MillPlanarGui","主要");
    CreatePage("MillPlanarGui", "几何体");
    CreatePage("MillPlanarGui", "刀轴和刀具补偿");
	CreatePage("MillPlanarGui", "进给率和速度");
	CreatePage("MillPlanarGui", "切削区域");
	CreatePage("MillPlanarGui", "策略");
	CreatePage("MillPlanarGui", "连接");
	CreatePage("MillPlanarGui", "进刀");
	CreatePage("MillPlanarGui", "退刀");
	CreatePage("MillPlanarGui", "转移快速");
	BindingEvents();
    SetQss();
	InitialMainPageParameter();
}

void CAM::MillPlanarGui::CreateTreeWidget()
{
    
    int width = dockWidget->width();
    int height = dockWidget->height();
    treeWidget->setGeometry(0, 0, dockWidget->width(), 750);
    treeWidget->setAlternatingRowColors(true);
    /*root*/
    widgetItemMap["Main"] = new QTreeWidgetItem(treeWidget);
    widgetItemMap["Main"]->setText(0, QString::fromLocal8Bit("主要"));
    widgetItemMap["Geometry"] = new QTreeWidgetItem(treeWidget);
    widgetItemMap["Geometry"]->setText(0, QString::fromLocal8Bit("几何体"));
    widgetItemMap["toolCompensation"] = new QTreeWidgetItem(treeWidget);
    widgetItemMap["toolCompensation"]->setText(0, QString::fromLocal8Bit("刀轴和刀具补偿"));
    widgetItemMap["Feedrate-Speed"] = new QTreeWidgetItem(treeWidget);
    widgetItemMap["Feedrate-Speed"]->setText(0, QString::fromLocal8Bit("进给率和速度"));
    widgetItemMap["MillArea"] = new QTreeWidgetItem(treeWidget);
    widgetItemMap["MillArea"]->setText(0, QString::fromLocal8Bit("切削区域"));
    widgetItemMap["Strategy"] = new QTreeWidgetItem(treeWidget);
    widgetItemMap["Strategy"]->setText(0, QString::fromLocal8Bit("策略"));
    widgetItemMap["Connect"] = new QTreeWidgetItem(treeWidget);
    widgetItemMap["Connect"]->setText(0, QString::fromLocal8Bit("链接"));
    widgetItemMap["non-cutting-movement"] = new QTreeWidgetItem(treeWidget);
    widgetItemMap["non-cutting-movement"]->setText(0, QString::fromLocal8Bit("非切削移动"));
    widgetItemMap["Tolerances-safety-distances"] = new QTreeWidgetItem(treeWidget);
    widgetItemMap["Tolerances-safety-distances"]->setText(0, QString::fromLocal8Bit("公差和安全距离"));
    widgetItemMap["CollisionDetection"] = new QTreeWidgetItem(treeWidget);
    widgetItemMap["CollisionDetection"]->setText(0, QString::fromLocal8Bit("碰撞检测"));
    widgetItemMap["Tool-Program-Machine-Control"] = new QTreeWidgetItem(treeWidget);
    widgetItemMap["Tool-Program-Machine-Control"]->setText(0, QString::fromLocal8Bit("刀具 程序和机床控制"));
    widgetItemMap["option"] = new QTreeWidgetItem(treeWidget);
    widgetItemMap["option"]->setText(0, QString::fromLocal8Bit("选项"));
    /*非切削移动子项*/
    widgetItemMap["Feed"] = new QTreeWidgetItem(widgetItemMap["non-cutting-movement"]);
    widgetItemMap["Feed"]->setText(0, QString::fromLocal8Bit("进刀"));
    widgetItemMap["tool-retracting"] = new QTreeWidgetItem(widgetItemMap["non-cutting-movement"]);
    widgetItemMap["tool-retracting"]->setText(0, QString::fromLocal8Bit("退刀"));
    //widgetItemMap["start-point"] = new QTreeWidgetItem(widgetItemMap["non-cutting-movement"]);
    //widgetItemMap["start-point"]->setText(0, QString::fromLocal8Bit("起点/钻点"));
    widgetItemMap["quick-transfer"] = new QTreeWidgetItem(widgetItemMap["non-cutting-movement"]);
    widgetItemMap["quick-transfer"]->setText(0, QString::fromLocal8Bit("转移/快速"));
    widgetItemMap["avoid"] = new QTreeWidgetItem(widgetItemMap["non-cutting-movement"]);
    widgetItemMap["avoid"]->setText(0, QString::fromLocal8Bit("避让"));

    for (auto iter=widgetItemMap.begin();iter!=widgetItemMap.end();iter++)
    {
        
        
    }

    connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(StackedWidgetPageChange()));




}


void CAM::MillPlanarGui::InitialMainPageParameter()
{

	/*读取data.ini*/
	/*lineEdit参数初始化设置*/
	/*主要*/
	settings->beginGroup("main");
	QStringList keys = settings->allKeys();
    for (const QString& key : keys)

    {
		QString value = settings->value(key).toString();
		QStringList splitValue = value.split("-");
        LineEditMap[splitValue[0].toLocal8Bit().data()]->setText(splitValue[1]);
		LineEditMap[splitValue[0].toLocal8Bit().data()]->setAlignment(Qt::AlignRight);
    }
    settings->endGroup();
	
    /*刀具*/
	settings->beginGroup("tool");
    /*获取刀具组中的所有刀具的键*/
	keys = settings->allKeys();
	MillGui::SettingSort(keys);
	ComboBoxMap["刀具"]->clear();
	for (const QString& key : keys) 
    {
        QString value = settings->value(key).toString();
		QStringList splitValue = value.split("-");
        ComboBoxMap["刀具"]->addItem(splitValue[1]);
	}
    settings->endGroup();

}

void CAM::MillPlanarGui::SetQss()
{
	/*读取QSS*/
	QString styleSheet;
	QFile file(":/QSS/CAM.qss");
	if (file.open(QFile::ReadOnly)) {
		styleSheet = QLatin1String(file.readAll());
		file.close();
	}
	for (auto iter = ComboBoxMap.begin(); iter != ComboBoxMap.end(); iter++)
	{
		ComboBoxMap[iter->first]->setStyleSheet(styleSheet);
	}
	for (auto iter = pushButtonMap.begin(); iter != pushButtonMap.end(); iter++)
	{
		pushButtonMap[iter->first]->setStyleSheet(styleSheet);
	}
    /*全局QSS设置*/
    setStyleSheet(styleSheet);
	/*设置窗口显示在最前面*/
	setWindowFlags(Qt::WindowFlags() | Qt::WindowStaysOnTopHint);
}

void CAM::MillPlanarGui::BindingEvents()
{
	connect(this->ComboBoxMap["切削模式"], SIGNAL(currentIndexChanged(int)), this, SLOT(PushButtonSet()));
	connect(this->pushButtonMap["切削模式-选取轮廓"], SIGNAL(clicked()), this, SLOT(SelectMillContour()));

}

void CAM::MillPlanarGui::SelectCuttingArea()
{
	Standard_Real color[3]{ 255,215,0 };
	viewerWindow->DisplayCoreManager->Context->InitDetected();
	viewerWindow->interactivemanager->SetSelectionModel(4);
	viewerWindow->interactivemanager->UnHighlight();
	auto object1 = viewerWindow->myOccView;
	auto object2 = viewerWindow->interactivemanager;
	connect(object1, SIGNAL(mouseMoveSignal(int)), this, SLOT(captureFace()));
	connect(object1, SIGNAL(mousePressSignal(int)), this, SLOT(GetShapeSurface()));
	connect(object1, SIGNAL(mousePressAndShiftSignal(int)), this, SLOT(RemoveSelectedShape()));
	this->hide();
}

void CAM::MillPlanarGui::GetShapeSurface()
{
    millPlanar->GetShapeSurface();
	viewerWindow->interactivemanager->ClearHilightObject();
    disconnect(viewerWindow->myOccView, SIGNAL(mousePressSignal(int)), nullptr, nullptr);
	disconnect(viewerWindow->myOccView, SIGNAL(mouseMoveSignal(int)), nullptr, nullptr);
	disconnect(viewerWindow->myOccView, SIGNAL(mousePressAndShiftSignal(int)), nullptr, nullptr);
	this->show();
}

void CAM::MillPlanarGui::GetSelectMillContour()
{
	viewerWindow->interactivemanager->GetMouseClicekedShapes();
	selectCurveGui->ShowSelectCurveNum();
}

void CAM::MillPlanarGui::SpecifyPart()
{
	Standard_Real color[3]{ 255,215,0 };
	viewerWindow->DisplayCoreManager->Context->InitDetected();
	viewerWindow->interactivemanager->SetSelectionModel(6);
    viewerWindow->interactivemanager->Highlight();
	auto object1 = viewerWindow->myOccView;
	auto object2 = viewerWindow->interactivemanager;
	//connect(object1, SIGNAL(mouseMoveSignal(int)), this, SLOT(captureShape()));
	connect(object1, SIGNAL(mousePressSignal(int)), this, SLOT(GetSelsctPart()));
	/*隐藏窗口*/
	this->hide();
}

void CAM::MillPlanarGui::captureCurve()
{
	viewerWindow->interactivemanager->captureShape(AllCurve);
}

void CAM::MillPlanarGui::captureFace()
{
	//viewerWindow->interactivemanager->captureShape(TopAbs_FACE);
}

void CAM::MillPlanarGui::GetSelsctPart()
{
    /*或许要加工的零件*/
	this->millPlanar->millPart = viewerWindow->interactivemanager->GetMouseClicekedShape();
	this->viewerWindow->interactivemanager->UnHighlight();
	auto object1 = viewerWindow->myOccView;
	disconnect(object1, SIGNAL(mouseMoveSignal(int)), nullptr, nullptr);
    disconnect(object1, SIGNAL(mousePressSignal(int)), nullptr, nullptr);
    this->show();
    
}

void CAM::MillPlanarGui::CloseSettingWindow()
{
    this->close();
}

void CAM::MillPlanarGui::CreateWCS()
{
    WcsSettingGui* wcsSettingGui = new WcsSettingGui();
    wcsSettingGui->SetupUi();
    wcsSettingGui->Init(viewerWindow, millPlanar, this);
    wcsSettingGui->show();
    this->hide();
}

void CAM::MillPlanarGui::PushButtonSet()
{
	millPlanar->ClearToolPath();
	viewerWindow->interactivemanager->ClearHilightObjects();
	viewerWindow->interactivemanager->ClearHilightObject();
	if (ComboBoxMap["切削模式"]->currentIndex() == 2)
	{
		pushButtonMap["切削模式-选取轮廓"]->setEnabled(true);
	}
	else
	{
		pushButtonMap["切削模式-选取轮廓"]->setEnabled(false);

	}
}

void CAM::MillPlanarGui::SelectMillContour()
{
	selectCurveGui = new SelectCurveGui();
	selectCurveGui->Init(viewerWindow, millPlanar, this);
	selectCurveGui->SetupUi();

	Standard_Real color[3]{ 255,215,0 };
	viewerWindow->DisplayCoreManager->Context->Deactivate();
	viewerWindow->DisplayCoreManager->Context->InitDetected();
	viewerWindow->interactivemanager->SetSelectionModel(2);
	viewerWindow->interactivemanager->UnHighlight();
	auto object1 = viewerWindow->myOccView;
	auto object2 = viewerWindow->interactivemanager;
	connect(object1, SIGNAL(mouseMoveSignal(int)), this, SLOT(captureCurve()));
	connect(object1, SIGNAL(mousePressSignal(int)), this, SLOT(GetSelectMillContour()));
	//connect(object1, SIGNAL(mousePressSignal(int)), selectCurveGui, SLOT(ShowSelectCurveNum()));
	connect(object1, SIGNAL(mousePressAndShiftSignal(int)), this, SLOT(RemoveSelectedShape()));
	selectCurveGui->show();
	this->hide();
}

void CAM::MillPlanarGui::RemoveSelectedShape()
{
	viewerWindow->interactivemanager->UnHilightSelectShape();
	selectCurveGui->ShowSelectCurveNum();
}

void CAM::MillPlanarGui::StackedWidgetPageChange()
{
    QTreeWidgetItem* item = treeWidget->currentItem();
    auto itemData = item->data(0, 0).toString();
    if (itemData== QString::fromLocal8Bit("主要"))
    {
        stackedWidget->setCurrentIndex(0);
    }
    else if ((itemData == QString::fromLocal8Bit("几何体")))
    {
        stackedWidget->setCurrentIndex(1);
    }

	else if ((itemData == QString::fromLocal8Bit("刀轴和刀具补偿")))
	{
		stackedWidget->setCurrentIndex(2);
	}
	else if ((itemData == QString::fromLocal8Bit("进给率和速度")))
	{
		stackedWidget->setCurrentIndex(3);
	}

	else if ((itemData == QString::fromLocal8Bit("切削区域")))
	{
		stackedWidget->setCurrentIndex(4);
	}
	else if ((itemData == QString::fromLocal8Bit("策略")))
	{
		stackedWidget->setCurrentIndex(5);
	}
	else if ((itemData == QString::fromLocal8Bit("链接")))
	{
		stackedWidget->setCurrentIndex(6);
	}
	else if ((itemData == QString::fromLocal8Bit("进刀")))
	{
		stackedWidget->setCurrentIndex(7);
	}

	else if ((itemData == QString::fromLocal8Bit("退刀")))
	{
		stackedWidget->setCurrentIndex(8);
	}
	else if ((itemData == QString::fromLocal8Bit("转移/快速")))
	{

		stackedWidget->setCurrentIndex(9);
	}
}


CAM::bottomBorderBarWidget::bottomBorderBarWidget()
{
    widget = new QWidget(this);
    QSpacerItem*horizontalSpacer = new QSpacerItem(20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    addWidget(widget);

    HBOX = new QHBoxLayout();
    HBOXLeft = new QHBoxLayout();
    HBOXCenter = new QHBoxLayout();
    HBOXRight = new QHBoxLayout();

    widget->setLayout(HBOX);
    HBOX->addItem(horizontalSpacer);
    HBOX->addLayout(HBOXRight, 0);
    setMovable(false);
    

}

void CAM::bottomBorderBarWidget::addPushbutton(QPushButton* button, Qt::Alignment alignment )
{
    HBOXRight->addWidget(button,0, alignment);
}

CAM::operateBorderBarWidget::operateBorderBarWidget()
{
	widget = new QWidget(this);
	QSpacerItem* horizontalSpacer = new QSpacerItem(20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
	addWidget(widget);

	HBOX = new QHBoxLayout();
	HBOXLeft = new QHBoxLayout();
	HBOXCenter = new QHBoxLayout();
	HBOXRight = new QHBoxLayout();

	widget->setLayout(HBOX);
	HBOX->addItem(horizontalSpacer);
    HBOX->addLayout(HBOXLeft, 0);
    HBOX->addLayout(HBOXCenter, 0);
    HBOX->addItem(horizontalSpacer);
	HBOX->addLayout(HBOXRight, 0);
	setMovable(false);
}

void CAM::operateBorderBarWidget::addPushbutton(QPushButton* button, Qt::Alignment alignment)
{
    HBOXCenter->addWidget(button, 0, alignment);
}


CAM::WcsSettingGui::WcsSettingGui()
{
}


void CAM::WcsSettingGui::Init(PartSolution* par1, MillPlanar* par2, MillPlanarGui* par3)
{
    viewerWindow = par1;
    millPlanar = par2;
    millPlanarGui = par3;
}

void CAM::WcsSettingGui::SetupUi()
{
    
    setGeometry(QRect(150, 150, 325, 325));
	stackedWidget = new QStackedWidget(this);
	stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
	stackedWidget->setGeometry(QRect(150, 10, 200, 150));
	stackedWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(240, 240, 240);"));
   

	ToolBarMap["bottomtoolbar"] = new bottomBorderBarWidget();
	addToolBar(Qt::BottomToolBarArea, ToolBarMap["bottomtoolbar"]);
	
	pushButtonMap["ok"] = new QPushButton();
	pushButtonMap["ok"]->setGeometry(QRect(0, 0, 90, 30));
	pushButtonMap["ok"]->setText(QString::fromLocal8Bit("确定"));

	pushButtonMap["cancel"] = new QPushButton();
	pushButtonMap["cancel"]->setGeometry(QRect(0, 0, 90, 30));
	pushButtonMap["cancel"]->setText(QString::fromLocal8Bit("取消"));

	ToolBarMap["bottomtoolbar"]->addPushbutton(pushButtonMap["ok"], Qt::AlignLeft);
	ToolBarMap["bottomtoolbar"]->addPushbutton(pushButtonMap["cancel"], Qt::AlignRight);
    setCentralWidget(stackedWidget);
	CreatePage("SettingsDialog", "设置坐标系");
	CreatePage("SettingsDialog", "建立坐标系");
    SetQss();
	
    connect(ComboBoxMap["坐标系选择-设置坐标系"], SIGNAL(currentIndexChanged(int)), this, SLOT(StackedWidgetPageChange()));
    connect(ComboBoxMap["坐标系选择-建立坐标系"], SIGNAL(currentIndexChanged(int)), this, SLOT(StackedWidgetPageChange()));
    connect(pushButtonMap["ok"], SIGNAL(clicked()), this, SLOT(CloseWindow()));
    connect(pushButtonMap["cancel"], SIGNAL(clicked()), this, SLOT(CloseWindow()));
    connect(pushButtonMap["原点-指定点"], SIGNAL(clicked()), this, SLOT(SpecifyPoint()));
    connect(pushButtonMap["X轴-指定矢量"], SIGNAL(clicked()), this, SLOT(SpecifyVectorX()));
    connect(pushButtonMap["Y轴-指定矢量"], SIGNAL(clicked()), this, SLOT(SpecifyVectorY()));
    connect(pushButtonMap["X轴-反向"], SIGNAL(clicked()), this, SLOT(ReverseVectorX()));
	connect(pushButtonMap["Y轴-反向"], SIGNAL(clicked()), this, SLOT(ReverseVectorY()));


}



void CAM::WcsSettingGui::SetQss()
{
	/*读取QSS*/
	QString styleSheet;
	QFile file(":/QSS/CAMDialog.qss");
	if (file.open(QFile::ReadOnly)) {
		styleSheet = QLatin1String(file.readAll());
		file.close();
	}

	for (auto iter = ComboBoxMap.begin(); iter != ComboBoxMap.end(); iter++)
	{
		ComboBoxMap[iter->first]->setStyleSheet(styleSheet);
	}
	for (auto iter = pushButtonMap.begin(); iter != pushButtonMap.end(); iter++)
	{
		pushButtonMap[iter->first]->setStyleSheet(styleSheet);
	}
	/*设置窗口显示在最前面*/
	setWindowFlags(Qt::WindowFlags() | Qt::WindowStaysOnTopHint);
}

void CAM::WcsSettingGui::CloseWindow()
{
    millPlanar->RemoveAllVector();
    this->millPlanarGui->show();
    this->close();
}

void CAM::WcsSettingGui::SpecifyPoint()
{
    /*设置选择过滤器*/
	viewerWindow->DisplayCoreManager->Context->Deactivate();
    viewerWindow->interactivemanager->SetSelectionModel(2);
    viewerWindow->interactivemanager->UnHighlight();
    auto object1 = viewerWindow->myOccView;
    auto object2 = viewerWindow->interactivemanager;
    connect(object1, SIGNAL(mouseMoveSignal(int)), this, SLOT(capturePoint()));
    connect(object1, SIGNAL(mousePressSignal(int)), this, SLOT(GetOrigin()));
    /*隐藏窗口*/
    this->hide();

}

void CAM::WcsSettingGui::SpecifyVectorX()
{
    Standard_Real color[3]{ 255,215,0 };
	viewerWindow->DisplayCoreManager->Context->Deactivate();
    viewerWindow->DisplayCoreManager->Context->InitDetected();
    viewerWindow->interactivemanager->SetSelectionModel(2);
    viewerWindow->interactivemanager->SetSelectionModel(4);
    viewerWindow->interactivemanager->UnHighlight();
	auto object1 = viewerWindow->myOccView;
	auto object2 = viewerWindow->interactivemanager;
	connect(object1, SIGNAL(mouseMoveSignal(int)), this, SLOT(captureShape()));
    connect(object1, SIGNAL(mousePressSignal(int)), this, SLOT(GetVectorX()));
	/*隐藏窗口*/
	this->hide();
   
}

void CAM::WcsSettingGui::SpecifyVectorY()
{

	Standard_Real color[3]{ 255,215,0 };
	viewerWindow->DisplayCoreManager->Context->Deactivate();
	viewerWindow->DisplayCoreManager->Context->InitDetected();
	viewerWindow->interactivemanager->SetSelectionModel(2);
	viewerWindow->interactivemanager->SetSelectionModel(4);
	viewerWindow->interactivemanager->UnHighlight();
	auto object1 = viewerWindow->myOccView;
	auto object2 = viewerWindow->interactivemanager;
	connect(object1, SIGNAL(mouseMoveSignal(int)), this, SLOT(captureShape()));
	connect(object1, SIGNAL(mousePressSignal(int)), this, SLOT(GetVectorY()));
	/*隐藏窗口*/
	this->hide();
}


void CAM::WcsSettingGui::capturePoint()
{
    viewerWindow->interactivemanager->capturePoint();
    
}

void CAM::WcsSettingGui::captureShape()
{
    viewerWindow->interactivemanager->captureShape(GeomAbs_Line);
}

void CAM::WcsSettingGui::GetOrigin()
{
   
   this->millPlanar->originPointShape =viewerWindow->interactivemanager->GetMouseClicekedShape();
   this->viewerWindow->interactivemanager->ClearHilightObject();
   millPlanar->CreateoriginPoint();
   auto object1 = viewerWindow->myOccView;
   disconnect(object1, SIGNAL(mouseMoveSignal(int)), nullptr, nullptr);
   disconnect(object1, SIGNAL(mousePressSignal(int)), nullptr, nullptr);
   millPlanar->CreateWCS();
   this->show();
}

void CAM::WcsSettingGui::GetVectorX()
{
    if (millPlanar->VectorXDisPlayed != 0)
    {
        viewerWindow->DisplayCoreManager->RemoveAxis(X_Vector);
        millPlanar->VectorXDisPlayed = 0;
        ReverseX = 1;
    }
    this->millPlanar->directionXShape = viewerWindow->interactivemanager->GetMouseClicekedShape();
    this->viewerWindow->interactivemanager->ClearHilightObject();
    millPlanar->CreateVector(X_Vector);
	auto object1 = viewerWindow->myOccView;
	disconnect(object1, SIGNAL(mouseMoveSignal(int)), nullptr, nullptr);
	disconnect(object1, SIGNAL(mousePressSignal(int)), nullptr, nullptr);
    millPlanar->CreateWCS();
	this->show();

}

void CAM::WcsSettingGui::GetVectorY()
{
	if (millPlanar->VectorYDisPlayed != 0)
	{
		viewerWindow->DisplayCoreManager->RemoveAxis(Y_Vector);
		millPlanar->VectorYDisPlayed = 0;
        ReverseY = 1;
	}
    this->millPlanar->directionYShape = viewerWindow->interactivemanager->GetMouseClicekedShape();
    this->viewerWindow->interactivemanager->ClearHilightObject();
	auto object1 = viewerWindow->myOccView;
    millPlanar->CreateVector(Y_Vector);
	disconnect(object1, SIGNAL(mouseMoveSignal(int)), nullptr, nullptr);
	disconnect(object1, SIGNAL(mousePressSignal(int)), nullptr, nullptr);
    millPlanar->CreateWCS();
	this->show();
    
}


void CAM::WcsSettingGui::ReverseVectorX()
{
    ReverseX *= -1;
    millPlanar->ReverseVector(X_Vector,ReverseX);
}

void CAM::WcsSettingGui::ReverseVectorY()
{
	ReverseY *= -1;
    millPlanar->ReverseVector(Y_Vector,ReverseY);
}

void CAM::WcsSettingGui::StackedWidgetPageChange()
{
    int index{ -1 };

    if (stackedWidget->currentIndex()==0)
	{
		   index = this->ComboBoxMap["坐标系选择-设置坐标系"]->currentIndex();
           this->ComboBoxMap["坐标系选择-设置坐标系"]->setCurrentIndex(index);

    }
    else if (stackedWidget->currentIndex() == 1)
    {
            index = this->ComboBoxMap["坐标系选择-建立坐标系"]->currentIndex();
            this->ComboBoxMap["坐标系选择-建立坐标系"]->setCurrentIndex(index);
    }
    
	if (index==0)
	{
		stackedWidget->setCurrentIndex(0);
	}
	else if (index == 2)
	{
		stackedWidget->setCurrentIndex(1);
	}

    millPlanar->ParaInit();

}



CAM::PageBuilder::PageBuilder()
{

}

void CAM::PageBuilder::Init(MillGui* theparent)
{
    parent = theparent;
}

void CAM::PageBuilder::Build()
{
    page = new QWidget();
    Scroll= new QScrollArea(parent);// 创建滚动类
    page->setMaximumHeight(726);
    page->setObjectName(QString::fromUtf8("page"));
	parent->stackedWidget->addWidget(Scroll);
    page->setLayout(pageQVBoxLayout);
    pageQVBoxLayout = new QVBoxLayout(page);
    pageQVBoxLayout->setSpacing(10);
    parent->stackedWidget->setCurrentIndex(0);
    Scroll->setWidget(page);
    Scroll->setMaximumHeight(800);
    Scroll->setWidgetResizable(true);

}

void CAM::PageBuilder::AddPanel(string itemNme)
{
	/*主要标题*/
	parent->LableMap[itemNme] = new QLabel(QString::fromLocal8Bit(itemNme.c_str()), page);
    PageQHBoxLayoutMap[itemNme] = new QHBoxLayout();
	PageQHBoxLayoutMap[itemNme]->setContentsMargins(0, 0, 0, 0);
	PageQHBoxLayoutMap[itemNme]->addWidget(parent->LableMap[itemNme], 0, Qt::AlignTop);
	PageQHBoxLayoutMap[itemNme]->setContentsMargins(0, 0, 0, 0);
    pageQVBoxLayout->addLayout(PageQHBoxLayoutMap[itemNme]);

}
void CAM::PageBuilder::CreateItem()
{
}
/*生成一行控件*/
void CAM::PageBuilder::AddItem(string panelName, list <PageItem> itemList)
{
    string itemLable{"None"};
    for (auto iter=itemList.begin();iter!=itemList.end();iter++)
    {
        

        if (iter->itemKind==("QLabel"))
        {
            itemLable = iter->lable;
            PageQHBoxLayoutMap[itemLable] = new QHBoxLayout();
            parent->LableMap[itemLable] = new QLabel(QString::fromLocal8Bit(itemLable.c_str()));
            PageQHBoxLayoutMap[itemLable]->addWidget(parent->LableMap[itemLable], 0, Qt::AlignBottom);
        }

        else if (iter->itemKind==("QSpacerItem"))
        {
			if (itemLable!="None")
			{
				SpacerItemMap[itemLable] = new QSpacerItem(120, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
				PageQHBoxLayoutMap[itemLable]->addItem(SpacerItemMap[itemLable]);
			}
			else
			{
				itemLable = panelName +"-"+iter->lable;
				SpacerItemMap[itemLable] = new QSpacerItem(120, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
				PageQHBoxLayoutMap[itemLable]->addItem(SpacerItemMap[itemLable]);
			}
          
        }
        else if(iter->itemKind==("QPushButton"))
        {
			if (itemLable!="None")
			{
				if (iter->lable.find("-")== iter->lable.npos)
				{
					string pushButtonName = itemLable + "-" + iter->lable;
					parent->pushButtonMap[pushButtonName] = new QPushButton(page);
					parent->pushButtonMap[pushButtonName]->setText(QString::fromLocal8Bit(iter->lable.c_str()));
					PageQHBoxLayoutMap[itemLable]->addWidget(parent->pushButtonMap[pushButtonName], 0);
				}
				else if (iter->lable.find("-") != 0)
				{
					QString lableResult = QString::fromLocal8Bit(iter->lable.c_str());
					QStringList lableList = lableResult.split("-");
					string pushButtonName = itemLable + "-" + lableList[0].toLocal8Bit().data();
					parent->pushButtonMap[pushButtonName] = new QPushButton(page);
					parent->pushButtonMap[pushButtonName]->setText(lableList[0]);

					if (lableList[1]=="disable")
					{
						parent->pushButtonMap[pushButtonName]->setEnabled(false);
					}
					PageQHBoxLayoutMap[itemLable]->addWidget(parent->pushButtonMap[pushButtonName], 0);


				}
				
			}
			else
			{
                itemLable = panelName;
				string pushButtonName = panelName + "-" + iter->lable;
				parent->pushButtonMap[pushButtonName] = new QPushButton(page);
				PageQHBoxLayoutMap[itemLable] = new QHBoxLayout();
				parent->pushButtonMap[pushButtonName]->setText(QString::fromLocal8Bit(iter->lable.c_str()));
				PageQHBoxLayoutMap[itemLable]->addWidget(parent->pushButtonMap[pushButtonName], 0);
			}
            
        }
		else if (iter->itemKind==("QComboBox"))
		{
            if (itemLable!="None")
            {
				parent->ComboBoxMap[itemLable] = new QComboBox(page);
				PageQHBoxLayoutMap[itemLable]->addWidget(parent->ComboBoxMap[itemLable], 0);
				QString lableResult = QString::fromLocal8Bit(iter->lable.c_str());
				QStringList lableList = lableResult.split(".");
				parent->ComboBoxMap[itemLable]->addItems(lableList);
            }
            else
            {
                itemLable= panelName;
				parent->ComboBoxMap[itemLable] = new QComboBox(page);
                PageQHBoxLayoutMap[itemLable] = new QHBoxLayout();
				PageQHBoxLayoutMap[itemLable]->addWidget(parent->ComboBoxMap[itemLable], 0, Qt::AlignBottom);
				PageQHBoxLayoutMap[itemLable]->setContentsMargins(0, 0, 0, 0);
                pageQVBoxLayout->addLayout(PageQHBoxLayoutMap[itemLable],0);
				QString lableResult = QString::fromLocal8Bit(iter->lable.c_str());
				QStringList lableList = lableResult.split(".");
				parent->ComboBoxMap[itemLable]->addItems(lableList);
                break;

            }
            
           
		}
		else if (iter->itemKind == ("QLineEdit"))
		{
			parent->LineEditMap[iter->lable] = new QLineEdit(page);
			PageQHBoxLayoutMap[itemLable]->addWidget(parent->LineEditMap[iter->lable], 0);
            parent->LineEditMap[iter->lable]->setText("0.000");
            parent->LineEditMap[iter->lable]->setAlignment(Qt::AlignRight);
		}
		else if (iter->itemKind == ("QCheckBox"))
		{
            parent->CheckBoxMap[iter->lable] = new QCheckBox(QString::fromLocal8Bit(iter->lable.c_str()),page);
            PageQHBoxLayoutMap[iter->lable] = new QHBoxLayout();
			PageQHBoxLayoutMap[iter->lable]->addWidget(parent->CheckBoxMap[iter->lable]);
			pageQVBoxLayout->addLayout(PageQHBoxLayoutMap[iter->lable]);
            break;
		}
		
		else if (iter->itemKind==("QFrame"))
		{
			/*横线*/
            string id =to_string (FramelineMap.size());
            PageQHBoxLayoutMap[id] = new QHBoxLayout();
			FramelineMap[id] = new QFrame(page);
            FramelineMap[id]->setGeometry(QRect(170, 370, 118, 3));
            FramelineMap[id]->setFrameShape(QFrame::HLine);
            FramelineMap[id]->setFrameShadow(QFrame::Sunken);
            PageQHBoxLayoutMap[id]->addWidget(FramelineMap[id]);
            pageQVBoxLayout->addLayout(PageQHBoxLayoutMap[id]);
            break;

		}

        pageQVBoxLayout->addLayout(PageQHBoxLayoutMap[itemLable]);


    }
}

void CAM::PageBuilder::SetPanel(string Oldpanel, string NewPanel)
{
}

void CAM::PageBuilder::Finished()
{
	/*空的*/
    PageQHBoxLayoutMap["void"] = new QHBoxLayout(page);
    pageQVBoxLayout->addLayout(PageQHBoxLayoutMap["void"]);
	SpacerItemMap["empty"] = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    PageQHBoxLayoutMap["void"]->addItem(SpacerItemMap["empty"]);
}

CAM::PageItem::PageItem(string thelable, string theItemKind)
{
    lable = thelable;
    itemKind = theItemKind;
}

CAM::PageItem::PageItem(string thelable, string theItemKind, string theItemName)
{
	lable = thelable;
	itemKind = theItemKind;
    itemName = theItemName;
}

void CAM::PageItem::SetLable(string thelable)
{
    lable = thelable;
}

void CAM::PageItem::SetItemKind(string theItemKind)
{
    itemKind = theItemKind;
}

void CAM::PageItem::SetItemName(string theItemName)
{
    itemName = theItemName;
}

CAM::MillGui::MillGui()
{
}

void CAM::MillGui::CreatePage(const string pageFilePath, const string pageFileName)
{
	pageBuilderMap[pageFileName.c_str()] = new PageBuilder();
	pageBuilderMap[pageFileName.c_str()]->Init(this);
	pageBuilderMap[pageFileName.c_str()]->Build();


	list<PageItem>pageItemList;
	PageItem pageItem;

	/*初始化setting*/
	QString filepath;
	string  filename;
	filepath = QCoreApplication::applicationDirPath();
	filename = "/config/";
	filename += pageFilePath + "/";
	filename += pageFileName + ".ini";
	filepath += QString::fromLocal8Bit(filename.c_str());
	/*读取page配置文件*/
	QSettings* PageSettings = new QSettings(filepath, QSettings::IniFormat);
	PageSettings->setIniCodec("UTF-8");
	/*获取根组的所有子组*/
	QStringList groups;
	groups = PageSettings->childGroups();
	for (const QString& item : groups)
	{
		QStringList resultKey;
		QString panelName;
		PageSettings->beginGroup(item);
		QStringList keys = PageSettings->allKeys();
		/*重新排序*/
		SettingSort(keys);
		for (const QString& key : keys)
		{
			QString value = PageSettings->value(key).toString();
			resultKey = key.split(".");
			if (resultKey[0].toStdString() == "0")
			{
                
				if (!value.contains("-"))
				{
					pageBuilderMap[pageFileName.c_str()]->AddPanel(value.toLocal8Bit().data());

				}
				panelName = value;
				continue;
			}

			pageItem.SetLable(value.toLocal8Bit().data());
			pageItem.SetItemKind(resultKey[1].toStdString());
			pageItemList.push_back(pageItem);

			if (resultKey[1].toStdString() == "newline")
			{
				pageBuilderMap[pageFileName.c_str()]->AddItem(panelName.toLocal8Bit().data(), pageItemList);
				pageItemList.clear();
			}
		}

		PageSettings->endGroup();
	}

	pageBuilderMap[pageFileName.c_str()]->Finished();
}

void CAM::MillGui::SettingSort(QStringList& thekeys)
{
	QStringList newKeys, resultList;
	for (int i = 0; i < thekeys.size(); ++i)
	{

		newKeys.append(QString());
	}
	for (const QString& key : thekeys)
	{
		resultList = key.split(".");
		QString id = resultList[0];
		newKeys[id.toInt()] = key;
	}
	thekeys = newKeys;
}

CAM::SelectCurveGui::SelectCurveGui()
{
}

void CAM::SelectCurveGui::Init(PartSolution* par1, MillPlanar* par2, MillPlanarGui* par3)
{
	viewerWindow = par1;
	millPlanar = par2;
	millPlanarGui = par3;
}

void CAM::SelectCurveGui::SetupUi()
{
	setGeometry(QRect(150, 150, 325, 200));
	stackedWidget = new QStackedWidget(this);
	stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
	stackedWidget->setGeometry(QRect(150, 10, 200, 150));
	stackedWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(240, 240, 240);"));


	ToolBarMap["bottomtoolbar"] = new bottomBorderBarWidget();
	addToolBar(Qt::BottomToolBarArea, ToolBarMap["bottomtoolbar"]);

	pushButtonMap["ok"] = new QPushButton();
	pushButtonMap["ok"]->setGeometry(QRect(0, 0, 90, 30));
	pushButtonMap["ok"]->setText(QString::fromLocal8Bit("确定"));

	pushButtonMap["cancel"] = new QPushButton();
	pushButtonMap["cancel"]->setGeometry(QRect(0, 0, 90, 30));
	pushButtonMap["cancel"]->setText(QString::fromLocal8Bit("取消"));

	ToolBarMap["bottomtoolbar"]->addPushbutton(pushButtonMap["ok"], Qt::AlignLeft);
	ToolBarMap["bottomtoolbar"]->addPushbutton(pushButtonMap["cancel"], Qt::AlignRight);
	setCentralWidget(stackedWidget);
	CreatePage("SettingsDialog", "选取曲线");
	connect(pushButtonMap["ok"], SIGNAL(clicked()), this, SLOT(CloseWindow()));
	connect(pushButtonMap["cancel"], SIGNAL(clicked()), this, SLOT(CloseWindow()));
	SetQss();
}

void CAM::SelectCurveGui::SetQss()
{
	/*读取QSS*/
	QString styleSheet;
	QFile file(":/QSS/CAM.qss");
	if (file.open(QFile::ReadOnly)) {
		styleSheet = QLatin1String(file.readAll());
		file.close();
	}
	for (auto iter = ComboBoxMap.begin(); iter != ComboBoxMap.end(); iter++)
	{
		ComboBoxMap[iter->first]->setStyleSheet(styleSheet);
	}
	for (auto iter = pushButtonMap.begin(); iter != pushButtonMap.end(); iter++)
	{
		pushButtonMap[iter->first]->setStyleSheet(styleSheet);
	}
	/*全局QSS设置*/
	setStyleSheet(styleSheet);
	/*设置窗口显示在最前面*/
	setWindowFlags(Qt::WindowFlags() | Qt::WindowStaysOnTopHint);
}

void CAM::SelectCurveGui::CloseWindow()
{
	this->millPlanarGui->show();
	this->close();
}



void CAM::SelectCurveGui::ShowSelectCurveNum()
{

	Standard_Integer curveNum{ 0 };
	curveNum = Standard_Integer(viewerWindow->interactivemanager->selectedShapesList.size());
	string text = "已选取曲线(";
	string	num = to_string(curveNum);
	text = text + num + ")";
	this->pushButtonMap["已选取曲线-已选取曲线(0)"]->setText(QString::fromLocal8Bit(text.c_str()));

}
