#pragma once
#include"PartSolution.h"
#include"DisplayCore.h"
#include<qobject.h>
#include "CamGuiManager.h"
#include "MillPlanar.h"
#include <QSettings>

using CAM::MillPlanar;
using CAM::PartSolution;
using CAM::MillPlanarGui;

class CAM::MillPlanar;
class CAM::MillPlanarGui;

namespace CAM
{
	class CamWindowManager :public QObject
	{
		Q_OBJECT
	public:
		CamWindowManager(QWidget* par);
		CamWindowManager();
		void InitialSettings();

	public:
		QWidget* parent;
		PartSolution* viewerWindow;
		MillPlanarGui* myMillPlanarGui;
		MillPlanar* millPlanar;
		QSettings* settings;
	public slots:
		/*导入零件*/
		void ImportPart();
		/*打开零件*/
		void OpenPart();
		void CreateToolPath();
		
	};
}
