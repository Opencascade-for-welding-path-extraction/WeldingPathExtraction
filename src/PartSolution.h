#pragma once
#include <QMainWindow>
#include<QtCore/Qstring>
#include"occView.h"
#include "DisplayCore.h"
#include "InteractiveModule.h"

using CAM::DisplayCore;
using CAM::OccView;

class CAM::OccView;
class CAM::DisplayCore;

namespace CAM
{
    class PartSolution : public QMainWindow
    {
        Q_OBJECT

    public:
        PartSolution(QWidget* parent = nullptr);
        ~PartSolution();

    public:
        QWidget* parent;
        OccView* myOccView;
        DisplayCore* DisplayCoreManager;
        InteractiveManager* interactivemanager;
    public slots:
        /*导入零件*/
        void ImportPart();
        /*打开零件*/
        void OpenPart();

    };
}
