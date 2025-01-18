#pragma once
#include <QtWidgets>
#include<string>
#include "PartSolution.h"
#include<qobject.h>
#include <QDockWidget>

using namespace std;
using CAM::PartSolution;

class MainWindow;

class WindownsManager:public QObject
{
	Q_OBJECT
public:
	
	WindownsManager(MainWindow * par);
	int CreateNewWindown(string WindownName);
	int CreateNewWindown();
	
	
	QString GetCurrentWindown();

public:
	MainWindow* parent;//主窗口widget
	list<string> windowns_id_list{1};//窗口编号
	map<string, PartSolution*> windowns_name;//3dview窗口map
	QTabWidget *multiwindowstabwidget;
	string current_windown;

public:
	enum RibbonStatus{ Modeling ,sketcher};

public slots:

	int TabwidgetChangeEvent();
	int TabwidgetClickedEvent();
	int	CloseWindown();


};

