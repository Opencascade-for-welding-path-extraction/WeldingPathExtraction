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
	MainWindow* parent;//������widget
	list<string> windowns_id_list{1};//���ڱ��
	map<string, PartSolution*> windowns_name;//3dview����map
	QTabWidget *multiwindowstabwidget;
	string current_windown;

public:
	enum RibbonStatus{ Modeling ,sketcher};

public slots:

	int TabwidgetChangeEvent();
	int TabwidgetClickedEvent();
	int	CloseWindown();


};

