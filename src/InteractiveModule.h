#pragma once
#include "DisplayCore.h"
#include <opencascade/TopoDS.hxx>
#include <opencascade/AIS_InteractiveObject.hxx>
#include <opencascade/AIS_Point.hxx>
#include <opencascade/AIS_Circle.hxx>
#include <opencascade/AIS_Line.hxx>
#include <opencascade/AIS_Shape.hxx>

using CAM::DisplayCore;

class CAM::DisplayCore;

namespace CAM
{
	enum CaptureShapeType
	{
		GeomAbs_Line,
		GeomAbs_Circle,
		GeomAbs_Ellipse,
		GeomAbs_Hyperbola,
		GeomAbs_Parabola,
		GeomAbs_BezierCurve,
		GeomAbs_BSplineCurve,
		GeomAbs_OffsetCurve,
		GeomAbs_OtherCurve,
		AllCurve,
		TopAbs_FACE,
		VectorType,
		None
	};

	class InteractiveManager
	{
	public:
		InteractiveManager(DisplayCore* par);
		void SetSelectionModel(int mode, const Handle(AIS_InteractiveObject) theObj);
		void SetSelectionModel(int mode);
		void SetHighlightStytle(Standard_Real color[3], int stylemode);
		void UnHighlight();
		void Highlight();
		void capturePoint();
		void captureShape(CaptureShapeType shapeType);
		void HilightCapturePoint(gp_Pnt apoint, Standard_Real distance, Standard_Real color[3]);
		void HilightCaptureShape(TopoDS_Shape shape, Standard_Real color[3]);
		void HilightCaptureCircle(TopoDS_Shape shape, Standard_Real color[3]);
		void HilightSelectedShape(TopoDS_Shape shape, Standard_Real color[3]);
		void UnHilightSelectShape();
		void ClearHilightObject();
		void ClearHilightObjects();
		TopoDS_Shape GetMouseClicekedShape();
		void GetMouseClicekedShapes();
		void GetMultipleEdge();


	public:
		DisplayCore* parent;
		Standard_Integer HilightPointDisplayed = 0;
		Standard_Integer HilightShapeDisplayed = 0;
		Handle(AIS_Point) captureHilightPoint { nullptr };//高亮point
		Handle(AIS_Circle) captureHilightCircle { nullptr };//高亮Circle
		Handle(AIS_Shape) captureHilightShape { nullptr };//高亮shape(edge)
		TopoDS_Shape currentHilightShape;//高亮shape(edge)
		TopoDS_Shape selectedShape;//获取shape(edge)
		vector<TopoDS_Shape> selectedShapesList;//获取shape(edge)
		vector<Handle(AIS_Shape)>hilightedShapeList;//单个高亮ais_shape
		vector<Handle(AIS_Shape)>hilightedShapesList;//多个高亮ais_shapes
		vector<Handle(AIS_Circle)>hilightedCircleList;
		vector<Handle(AIS_Point)>hilightedPointList;
		int HilightBrepLineID{ 1024 };
		int HilightBrepShapeID{ 0 };
		Standard_Real XP;
		Standard_Real YP;
		Standard_Real ZP;
		Standard_Real XV;
		Standard_Real YV;
		Standard_Real ZV;


	};
}