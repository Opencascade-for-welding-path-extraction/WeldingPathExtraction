#pragma once
#include "CamGuiManager.h"
#include "PartSolution.h"
#include <cmath>
#include <float.h>
#include <opencascade/Standard_Macro.hxx>
#include <opencascade/TopoDS_Face.hxx>
#include <opencascade/gp_Pnt.hxx>

using CAM::PartSolution;
class CAM::PartSolution;

/*定义加工类型的枚举*/
enum Millstrategy { ReciprocatingMill, ContourFollowingMill}; 
namespace CAM
{
	class MillPlanarGui;

	class MillPlanar
	{
	public:
		MillPlanar();
		void Init(MillPlanarGui* par1, PartSolution* par2);
		void CreateToolPath();
		void CreateToolPath(Standard_Real Length, Standard_Real Width);
		void GetShapeSurface();
		void GetContinuousWire();
		void IntelligentOffset();
		void ReciprocatingMill();
		void ContourFollowingMill();
		void ContourMill();
		void SetMillStrategy();
		void ClearToolPath();
		string GetPathWireID();
		void CreateWCS();
		void CreateoriginPoint();
		void CreateVector(Vector_Type thetype,Standard_Integer reversed=1);
		void ReverseVector(Vector_Type thetype, Standard_Integer thereversed);
		void ParaInit();
		void RemoveAllVector();
		TopoDS_Shape ConvertWCS(const TopoDS_Shape theshape);//转换为工作坐标系
		TopoDS_Shape ConvertACS(const TopoDS_Shape theshape);//转换为世界坐标系
		bool OutPutCLSFile();
		void CreateCLSF();
		void AddHelicalApproach();
		void AddLinerApproach();
		void AddRetract();

	public:
		TopoDS_Face millface;
		TopoDS_Shape millPart;
		list<TopoDS_Shape>PathWireList;
		map<string,TopoDS_Shape>PathWireMap;
		int PathWireID{0};
		gp_Pnt originPoint;
		gp_Dir directionX;
		gp_Dir directionY;
		gp_Ax3 WcsgpAx3;
		TopoDS_Shape originPointShape;
		TopoDS_Shape directionXShape;
		TopoDS_Shape directionYShape;
		TopoDS_Shape xVectorShape;
		TopoDS_Shape yVectorShape;

		Standard_Integer VectorXDisPlayed = 0;
		Standard_Integer VectorYDisPlayed = 0;
		Standard_Integer VectorZDisPlayed = 0;
		Standard_Integer originPointCreated = 0;
		Standard_Integer directionXCreated = 0;
		Standard_Integer directionYCreated = 0;

		gp_Pnt startPoint;
		gp_Pnt endPoint;
		
	public:
		MillPlanarGui* millPlanarGui;
		PartSolution* viewerWindow;
	};

}

