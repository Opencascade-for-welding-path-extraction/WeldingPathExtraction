#include "DisplayCore.h"
#include "InteractiveModule.h"
#include <opencascade/AIS_InteractiveObject.hxx>
#include <opencascade/AIS_Point.hxx>
#include <opencascade/AIS_Shape.hxx>
#include <opencascade/BRep_Tool.hxx>
#include <opencascade/BRepAdaptor_Curve.hxx>
#include <opencascade/BRepBuilderAPI_MakeEdge.hxx>
#include <opencascade/BRepExtrema_DistShapeShape.hxx>
#include <opencascade/BRepTools.hxx>
#include <opencascade/Geom_CartesianPoint.hxx>
#include <opencascade/Geom_Circle.hxx>
#include <opencascade/Geom_Line.hxx>
#include <opencascade/Geom_Surface.hxx>
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/Quantity_ColorRGBA.hxx>
#include <opencascade/Quantity_TypeOfColor.hxx>
#include <opencascade/Standard_Real.hxx>
#include <opencascade/StdSelect_BRepOwner.hxx>
#include <opencascade/TopAbs_ShapeEnum.hxx>
#include <opencascade/TopExp.hxx>
#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/TopoDS_Edge.hxx>
#include <opencascade/TopoDS_Face.hxx>
#include <opencascade/V3d_View.hxx>


#include <QDebug>
#include <qopengltexture.h>


class CAM::DisplayCore;

CAM::InteractiveManager::InteractiveManager(DisplayCore* par)
{
	parent = par;

}

void CAM::InteractiveManager::SetSelectionModel(int mode, const Handle(AIS_InteractiveObject) theObj)
/*
	TopAbs_VERTEX:    return 1; //选择顶点Shape或Shape的顶点
	TopAbs_EDGE:      return 2; //选择边Shape或Shape的边
	TopAbs_WIRE:      return 3; //选择环Shape或Shape的环
	TopAbs_FACE:      return 4; //选择面Shape或Shape的Face
	TopAbs_SHELL:     return 5; //选择Shell
	TopAbs_SOLID:     return 6;
	TopAbs_COMPSOLID: return 7;
	TopAbs_COMPOUND:  return 8; //选择复合Shape，效果同TopAbs_Shape
	TopAbs_SHAPE:     return 0; //选择一个Shape

*/
{
	parent->Context->SetAutomaticHilight(TRUE);
	//parent->Context->Deactivate();
	//parent->Context->SetAutoActivateSelection(TRUE);
	parent->Context->Activate(theObj, mode);
	


}

void CAM::InteractiveManager::SetSelectionModel(int mode)
/*
	TopAbs_VERTEX:    return 1; //选择顶点Shape或Shape的顶点
	TopAbs_EDGE:      return 2; //选择边Shape或Shape的边
	TopAbs_WIRE:      return 3; //选择环Shape或Shape的环
	TopAbs_FACE:      return 4; //选择面Shape或Shape的Face
	TopAbs_SHELL:     return 5; //选择Shell
	TopAbs_SOLID:     return 6;
	TopAbs_COMPSOLID: return 7;
	TopAbs_COMPOUND:  return 8; //选择复合Shape，效果同TopAbs_Shape
	TopAbs_SHAPE:     return 0; //选择一个Shape

*/
{
	parent->Context->SetAutomaticHilight(TRUE);
	parent->Context->Activate(mode);

	if (mode==2)
	{
		Handle(Prs3d_Drawer) drawer = parent->Context->HighlightStyle(Prs3d_TypeOfHighlight_LocalDynamic);
		Quantity_Color highlightColor(Quantity_NOC_RED);
		Handle(Prs3d_LineAspect) lineAspect = new Prs3d_LineAspect(highlightColor, Aspect_TOL_SOLID, 6.0);
		drawer->SetColor(highlightColor);
		drawer->SetWireAspect(lineAspect);
		drawer->SetUnFreeBoundaryAspect(lineAspect);
		drawer->SetFreeBoundaryAspect(lineAspect);
		parent->Context->SetHighlightStyle(Prs3d_TypeOfHighlight_LocalDynamic, drawer);
	}
	else if(mode==4)
	{
		
		//get default drawer from context
		//Handle(Prs3d_Drawer) drawer = parent->Context->DefaultDrawer();
		//set faces edges visible
		//drawer->SetFaceBoundaryDraw(true);
		auto view = parent->myOccView->getV3dview();
		Handle(Prs3d_Drawer) drawer = new Prs3d_Drawer();
		drawer->SetFaceBoundaryDraw(Standard_True); // 如果想要显示面边界
		drawer->SetColor(Quantity_NOC_BLUE); // 选择选中的高亮颜色
		parent->Context->SetHighlightStyle(Prs3d_TypeOfHighlight_Selected, drawer);
		parent->Context->HighlightStyle(Prs3d_TypeOfHighlight_Selected)->SetFaceBoundaryDraw(Standard_True);
		parent->Context->HighlightStyle(Prs3d_TypeOfHighlight_Selected)->SetColor(Quantity_NOC_BLUE);
		view->Redraw();
	}
	

}

void CAM::InteractiveManager::SetHighlightStytle(Standard_Real color[3], int stylemode)
{
	Handle(Prs3d_Drawer) drawer = parent->Context->HighlightStyle();
	drawer->SetDisplayMode(AIS_Shaded);
	drawer->SetColor(Quantity_Color(color[0] /255.0, color[1] /255.0, color[2]/255.0, Quantity_TypeOfColor::Quantity_TOC_RGB));
	//drawer->SetLineAspect(new Prs3d_LineAspect(Quantity_NOC_YELLOW, Aspect_TOL_SOLID, 3.0));
	parent->Context->SetHighlightStyle(drawer);
	parent->Context->SetToHilightSelected(true);
	parent->Context->SetHighlightStyle(Prs3d_TypeOfHighlight_Dynamic, drawer);
	parent->Context->UpdateCurrentViewer();
}

void CAM::InteractiveManager::Highlight()
{
	parent->Context->SetAutomaticHilight(TRUE);
	//auto color = Quantity_Color(0.5, 0.6, 0.7, Quantity_TypeOfColor::Quantity_TOC_RGB);
	//SetHighlightStytle(color, 1);
	//parent->Context->HilightSelected(TRUE);
}


void CAM::InteractiveManager::capturePoint()
{
	
	/*获取显示零件的轮廓线*/
	Handle(TopoDS_Face) selected__ais_face;
	vector<TopoDS_Edge>edgeList;
	parent->Context->InitDetected();
	Handle(Geom_Surface) aSurface;
	if (true)
	{
		Handle(AIS_InteractiveContext)& ctx = parent->Context;
		for (ctx->InitDetected(); ctx->MoreDetected(); ctx->NextDetected())
		{
			Handle(StdSelect_BRepOwner) owner = Handle(StdSelect_BRepOwner) ::DownCast(ctx->DetectedCurrentOwner());
			if (owner.IsNull())
				continue;
			try
			{
				TopoDS_Edge edge = TopoDS::Edge(owner->Shape());
				edgeList.push_back(edge);
				break;

			}
			catch (...)
			{


			}
			
		}
	}
	
	/*获取当前鼠标位置的点*/
	Standard_Real XP;
	Standard_Real YP;
	Standard_Real ZP;
	Standard_Real XV;
	Standard_Real YV;
	Standard_Real ZV;
	gp_Pnt currentPoint;
	gp_Dir currentDirection;
	auto X = parent->myOccView->_dragPosX;
	auto Y = parent->myOccView->_dragPosY;
	Handle(V3d_View) view = parent->myOccView->getV3dview();
	view->ProjReferenceAxe(X, Y, XP, YP, ZP, XV, YV, ZV);
	/*定义当前鼠标点的gp_pnt*/
	currentPoint.SetX(XP);
	currentPoint.SetY(YP);
	currentPoint.SetZ(ZP);
	/*定义鼠标当前位置的gp_dir*/
	currentDirection = gp_Dir(XV, YV, ZV);
	gp_Lin line = gp_Lin(currentPoint, currentDirection);
	BRepBuilderAPI_MakeEdge buildedge = BRepBuilderAPI_MakeEdge(line);
	TopoDS_Edge edge = buildedge.Edge();
	Standard_Real  minDistance{ 15 }, minimumDistance{ 0 };//距离鼠标点最近边的距离
	gp_Pnt nearest_point1;
	gp_Pnt nearest_point2;
	gp_Pnt hilight;

	int brepLineID = 1024;
	if (buildedge.IsDone())
	{
		for (int i = 0; i < edgeList.size(); i++)
		{
			TopoDS_Shape ashape = edgeList[i];
			BRepExtrema_DistShapeShape extrema = BRepExtrema_DistShapeShape(ashape, edge);
			nearest_point1 = extrema.PointOnShape1(1);
			nearest_point2 = extrema.PointOnShape2(1);
			minimumDistance = nearest_point1.Distance(nearest_point2);
			minDistance = minimumDistance;
			brepLineID = i;


		}


		if (minimumDistance >= 20 or minimumDistance == 0)//如果最小值大于20 则鼠标远离edge 更新界面
		{
			/*刷新界面*/
			parent->Context->UpdateCurrentViewer();
		}
	}
	if (brepLineID != 1024)
	{
		TopoDS_Shape ashape = edgeList[brepLineID];
		BRepExtrema_DistShapeShape extrema = BRepExtrema_DistShapeShape(ashape, edge);
		nearest_point1 = extrema.PointOnShape1(1);
		nearest_point2 = extrema.PointOnShape2(1);
		minimumDistance = nearest_point1.Distance(nearest_point2);

		/*创建 BRepAdaptor_Curve 对象*/
		BRepAdaptor_Curve curve(edgeList[brepLineID]);
		/*获取边的曲线类型*/
		GeomAbs_CurveType curveType = curve.GetType();
		// 检查曲线类型是否为圆
		if (curveType == GeomAbs_Circle)
		{

			// 如果是圆，还可以进一步获取圆的信息，比如圆心和半径
			gp_Circ circle = curve.Circle();
			// 可选：获取圆的相关信息
			gp_Pnt center = circle.Location();
			Standard_Real radius = circle.Radius();
			Standard_Real color[3] = { 255,215,0 };
			if (minimumDistance <= 5)
			{
				HilightCapturePoint(center, minimumDistance, color);
				HilightCaptureShape(edgeList[brepLineID], color);
				HilightBrepLineID = brepLineID;
			}

		}
		else
		{
			//获取边的起始和结束参数
			Standard_Real startParam = curve.FirstParameter();
			Standard_Real endParam = curve.LastParameter();
			auto FirstV = TopExp::FirstVertex(edgeList[brepLineID]);
			auto LastV = TopExp::LastVertex(edgeList[brepLineID]);


			/*端点1 gp_pnt*/
			gp_Pnt point1 = BRep_Tool::Pnt(FirstV);
			/*端点1 gp_pnt*/
			gp_Pnt point2 = BRep_Tool::Pnt(LastV);
			/*中点 gp_pnt*/
			Standard_Real Z;
			X = (point1.X() + point2.X()) / 2;
			Y = (point1.Y() + point2.Y()) / 2;
			Z = (point1.Z() + point2.Z()) / 2;
			gp_Pnt point3 = gp_Pnt(X, Y, Z);


			if (point1.Distance(nearest_point1) <= 10)
			{
				Standard_Real color[3] = { 255,215,0 };
				HilightCapturePoint(point1, minimumDistance, color);
				HilightBrepLineID = brepLineID;
			}

			else if (point2.Distance(nearest_point1) <= 10)
			{
				Standard_Real color[3] = { 255,215,0 };
				HilightCapturePoint(point2, minimumDistance, color);
				HilightBrepLineID = brepLineID;
			}

			else if (point3.Distance(nearest_point1) <= 10)
			{
				Standard_Real color[3] = { 255,215,0 };
				HilightCapturePoint(point3, minimumDistance, color);
				HilightBrepLineID = brepLineID;
			}

			else if (minimumDistance < 10 and false)
			{
				Standard_Real color[3] = { 0,0,0 };
				HilightCapturePoint(nearest_point1, minimumDistance, color);
				HilightBrepLineID = brepLineID;
			}

		}


	}
	else if (HilightPointDisplayed == 1 || HilightShapeDisplayed == 1)
	{
		if (hilightedPointList.size() == 2)
		{
			parent->Context->Remove(captureHilightPoint, Standard_True);
			hilightedPointList.erase(hilightedPointList.begin());
			HilightPointDisplayed = 0;
			captureHilightPoint = nullptr;
		}


		if (hilightedCircleList.size()!=0)
		{
			parent->Context->Remove(captureHilightCircle, Standard_True);
			hilightedCircleList.erase(hilightedCircleList.begin());
			HilightShapeDisplayed = 0;
			captureHilightCircle = nullptr;
		}
		

	}
	
		
	


	
}

void CAM::InteractiveManager::captureShape(CaptureShapeType shapeType)
{
	/*获取显示零件的轮廓线*/
	vector<TopoDS_Shape>ShapeList;
	TopoDS_Shape detectedShape;
	Handle(AIS_InteractiveContext)& ctx = parent->Context;
	for (ctx->InitDetected(); ctx->MoreDetected(); ctx->NextDetected())
	{
		Handle(StdSelect_BRepOwner) owner = Handle(StdSelect_BRepOwner) ::DownCast(ctx->DetectedCurrentOwner());
		if (owner.IsNull())
		{
			detectedShape.Nullify();
			continue;
		}

		else
		{
			if (owner->Shape().ShapeType() == TopAbs_EDGE and ShapeList.empty())
			{
				TopoDS_Edge edge = TopoDS::Edge(owner->Shape());
				detectedShape = owner->Shape();
				ShapeList.push_back(edge);
			}

			else if (owner->Shape().ShapeType() == TopAbs_ShapeEnum::TopAbs_FACE and ShapeList.empty())
			{
				TopoDS_Face face = TopoDS::Face(owner->Shape());
				detectedShape = owner->Shape();
				ShapeList.push_back(face);
			}
		}

		break;
	}

	if (ShapeList.size()==0 and hilightedShapeList.size()!=0 and detectedShape.IsNull())
	{
		parent->Context->Remove(hilightedShapeList[0], Standard_True);
		hilightedShapeList.erase(hilightedShapeList.begin());
		HilightShapeDisplayed = 0;
		parent->Context->UpdateCurrentViewer();
	}

	else if(ShapeList.size() != 0)
	{
		qDebug() << ShapeList[0].ShapeType()<<ShapeList.size();
		if (!ShapeList.empty() and ShapeList[0].ShapeType() == TopAbs_ShapeEnum::TopAbs_EDGE)
		{
			TopoDS_Edge ashape = TopoDS::Edge(ShapeList[0]);//高亮的Edge

			/*创建 BRepAdaptor_Curve 对象*/
			BRepAdaptor_Curve curve(ashape);
			/*获取边的曲线类型*/
			GeomAbs_CurveType curveType = curve.GetType();
			// 检查曲线类型是否为直线
			if (curveType == shapeType or shapeType ==AllCurve)
			{

				Standard_Real color[3] = { 255,215,0 };
				
				HilightCaptureShape(ashape, color);
				HilightBrepLineID = 0;
				if (hilightedShapeList.size() == 2 and detectedShape.IsNull())
				{
					parent->Context->Remove(hilightedShapeList[0], Standard_True);
					hilightedShapeList.erase(hilightedShapeList.begin());
					HilightShapeDisplayed = 0;
					parent->Context->UpdateCurrentViewer();
				}
			}

			if (HilightShapeDisplayed == 1)
			{
				parent->Context->Remove(captureHilightShape, Standard_True);
				HilightShapeDisplayed = 0;
				captureHilightShape = nullptr;
			}
			
		}
		
		else if (!ShapeList.empty() and ShapeList[0].ShapeType() == TopAbs_ShapeEnum::TopAbs_FACE)
		{
			/*刷新界面*/
			TopoDS_Face ashape = TopoDS::Face(ShapeList[0]);//高亮的Face
			Standard_Real color[3] = { 255,215,0 };
			HilightCaptureShape(ashape, color);
			if (HilightShapeDisplayed == 1)
			{
				if (hilightedShapeList.size()>=2 and detectedShape.IsNull())
				{
					parent->Context->Remove(hilightedShapeList[0], Standard_True);
					hilightedShapeList.erase(hilightedShapeList.begin());
					HilightShapeDisplayed = 0;
					parent->Context->UpdateCurrentViewer();
				}

			}

		}
		

	}
	
	
	
}


void CAM::InteractiveManager::HilightCapturePoint(gp_Pnt apoint, Standard_Real distance, Standard_Real color[3])
{
	Handle(Geom_Point) p = new Geom_CartesianPoint(apoint);
	if (!captureHilightPoint) captureHilightPoint = new AIS_Point(p);
	else captureHilightPoint->SetComponent(p);
	auto drawer = captureHilightPoint->Attributes();
	auto acolor = Quantity_Color(color[0] / 255, color[1] / 255, color[2] / 255, Quantity_TOC_RGB);
	Handle(Prs3d_PointAspect) asp = new Prs3d_PointAspect(Aspect_TOM_POINT, acolor, 6);
	drawer->SetPointAspect(asp);
	captureHilightPoint->SetAttributes(drawer);
	if (HilightPointDisplayed == 0)
	{
		parent->Context->Display(captureHilightPoint, Standard_True);
		hilightedPointList.push_back(captureHilightPoint);
		HilightPointDisplayed = 1;
	}
	else if (HilightPointDisplayed == 1)
	{
		parent->Context->Redisplay(captureHilightPoint, Standard_True);
	}

}

void CAM::InteractiveManager::HilightCaptureShape(TopoDS_Shape shape, Standard_Real color[3])
{
	if (!captureHilightShape)
		captureHilightShape = new AIS_Shape(shape);
	else
	{
		captureHilightShape->SetShape(shape);
	}
	auto drawer = captureHilightShape->Attributes();
	auto acolor = Quantity_Color(color[0] / 255, color[1] / 255, color[2] / 255, Quantity_TOC_RGB);
	//Handle(Prs3d_PointAspect) asp = new Prs3d_PointAspect(Aspect_TOM_POINT, acolor, 6);
	//drawer->SetPointAspect(asp);
	captureHilightShape->SetAttributes(drawer);
	captureHilightShape->SetColor(acolor);
	captureHilightShape->SetWidth(5);

	if (HilightShapeDisplayed == 0)
	{
		parent->Context->Display(captureHilightShape, Standard_True);
		HilightShapeDisplayed = 1;
		hilightedShapeList.push_back(captureHilightShape);
	}
	else if (HilightShapeDisplayed == 1)
	{
		parent->Context->Redisplay(captureHilightShape, Standard_True);
	}

}

void CAM::InteractiveManager::HilightCaptureCircle(TopoDS_Shape shape, Standard_Real color[3])
{
	if (shape.ShapeType() == TopAbs_EDGE)
	{
		GeomAbs_CurveType curveType;
		BRepAdaptor_Curve curve;
		TopoDS_Edge ashape = TopoDS::Edge(shape);//高亮的Edge
		if (!shape.IsNull())
		{
			curve = BRepAdaptor_Curve(ashape);
			curveType = curve.GetType();
			qDebug() << "GeomAbs_CurveType" << curveType;
		}

		if (curveType == GeomAbs_Circle)
		{
			gp_Circ circ = curve.Circle();
			Handle(Geom_Circle) geomCircle = new Geom_Circle(circ);
			if (!captureHilightCircle) captureHilightCircle = new AIS_Circle(geomCircle);
			auto drawer = captureHilightCircle->Attributes();
			auto acolor = Quantity_Color(color[0] / 255, color[1] / 255, color[2] / 255, Quantity_TOC_RGB);
			//Handle(Prs3d_PointAspect) asp = new Prs3d_PointAspect(Aspect_TOM_POINT, acolor, 6);
			//drawer->SetPointAspect(asp);
			captureHilightPoint->SetAttributes(drawer);
			captureHilightCircle->SetColor(acolor);
			captureHilightCircle->SetWidth(5);

			if (HilightShapeDisplayed == 0)
			{
				parent->Context->Display(captureHilightCircle, Standard_True);
				hilightedCircleList.push_back(captureHilightCircle);
				HilightShapeDisplayed = 1;
			}
			else if (HilightShapeDisplayed == 1)
			{
				parent->Context->Redisplay(captureHilightCircle, Standard_True);
			}
		}
		else if (curveType == GeomAbs_Line)
		{
			if (!captureHilightShape) captureHilightShape = new AIS_Shape(shape);
			auto drawer = captureHilightShape->Attributes();
			auto acolor = Quantity_Color(color[0] / 255, color[1] / 255, color[2] / 255, Quantity_TOC_RGB);
			//Handle(Prs3d_PointAspect) asp = new Prs3d_PointAspect(Aspect_TOM_POINT, acolor, 6);
			//drawer->SetPointAspect(asp);
			captureHilightShape->SetAttributes(drawer);
			captureHilightShape->SetColor(acolor);
			captureHilightShape->SetWidth(5);

			if (HilightShapeDisplayed == 0)
			{
				parent->Context->Display(captureHilightShape, Standard_True);
				HilightShapeDisplayed = 1;
			}
			else if (HilightShapeDisplayed == 1)
			{
				parent->Context->Redisplay(captureHilightShape, Standard_True);
			}

		}

	}
}

void CAM::InteractiveManager::HilightSelectedShape(TopoDS_Shape shape, Standard_Real color[3])
{
	Handle(AIS_Shape) HilightedShape = new AIS_Shape(shape);
	auto drawer = HilightedShape->Attributes();
	auto acolor = Quantity_Color(color[0] / 255, color[1] / 255, color[2] / 255, Quantity_TOC_RGB);
	//Handle(Prs3d_PointAspect) asp = new Prs3d_PointAspect(Aspect_TOM_POINT, acolor, 6);
	//drawer->SetPointAspect(asp);
	HilightedShape->SetAttributes(drawer);
	HilightedShape->SetColor(acolor);
	HilightedShape->SetWidth(4);
	parent->Context->Display(HilightedShape, Standard_True);
	hilightedShapesList.push_back(HilightedShape);

}

void CAM::InteractiveManager::UnHilightSelectShape()
{
	Handle(AIS_InteractiveContext)& ctx = parent->Context;
	Handle(StdSelect_BRepOwner) owner;
	TopoDS_Shape unSelectionShape;
	for (ctx->InitDetected(); ctx->MoreDetected(); ctx->NextDetected())
	{
		try
		{
			owner = Handle(StdSelect_BRepOwner) ::DownCast(ctx->DetectedCurrentOwner());

			if (owner.IsNull())
			{
				continue;
			}
			else
			{
				if ((owner->Shape().ShapeType() == TopAbs_EDGE))
				{
					unSelectionShape = owner->Shape();
				}
				

				break;
			}

		}
		catch (...)
		{

			selectedShape.Nullify();
		}

	}
	for (auto iter:hilightedShapesList)
	{
		if (iter->Shape().IsEqual(unSelectionShape))
		{
			parent->Context->Remove(iter, Standard_True);
			selectedShapesList.erase(find(selectedShapesList.begin(),selectedShapesList.end(), unSelectionShape));
			hilightedShapesList.erase(find(hilightedShapesList.begin(),hilightedShapesList.end(),iter));
			

		}

	}
}

void CAM::InteractiveManager::ClearHilightObject()
{
	if (hilightedShapeList.size()!=0)
	{
		parent->Context->Remove(captureHilightShape, Standard_True);
		hilightedShapeList.erase(hilightedShapeList.begin());
		HilightShapeDisplayed = 0;
	}
	if (hilightedCircleList.size()!=0)
	{
		parent->Context->Remove(captureHilightShape, Standard_True);
		hilightedCircleList.erase(hilightedCircleList.begin());
		HilightShapeDisplayed = 0;
	}
	
}

void CAM::InteractiveManager::ClearHilightObjects()
{
	for (auto iter : hilightedShapesList)
	{

			parent->Context->Remove(iter, Standard_True);
			
	}
	hilightedShapesList.clear();

}

TopoDS_Shape CAM::InteractiveManager::GetMouseClicekedShape()
{
	Handle(AIS_InteractiveContext)& ctx = parent->Context;
	Handle(StdSelect_BRepOwner) owner;
	//TopAbs_ShapeEnum ShapeType;
	for (ctx->InitDetected(); ctx->MoreDetected(); ctx->NextDetected())
	{
		try
		{
			owner = Handle(StdSelect_BRepOwner) ::DownCast(ctx->DetectedCurrentOwner());

			if (owner.IsNull())
			{
				continue;
			}
			else
			{
				selectedShape = owner->Shape();
				break;
			}
			
		}
		catch (...)
		{
			
			selectedShape.Nullify();
		}
		
	}
	
	
	return selectedShape;


	
	
}

void CAM::InteractiveManager::GetMouseClicekedShapes()
{
	
	Handle(AIS_InteractiveContext)& ctx = parent->Context;
	Handle(StdSelect_BRepOwner) owner;
	Standard_Real color[3] = { 0,139,69 };
	//TopAbs_ShapeEnum ShapeType;
	for (ctx->InitDetected(); ctx->MoreDetected(); ctx->NextDetected())
	{
		try
		{
			owner = Handle(StdSelect_BRepOwner) ::DownCast(ctx->DetectedCurrentOwner());

			if (owner.IsNull())
			{
				continue;
			}
			else
			{
				selectedShape = owner->Shape();
				selectedShapesList.push_back(selectedShape);
				qDebug() << "777";
				HilightSelectedShape(selectedShape, color);
				break;
			}

		}
		catch (...)
		{

			selectedShape.Nullify();
		}

	}


	
}

void CAM::InteractiveManager::GetMultipleEdge()
{

}



void CAM::InteractiveManager::UnHighlight()
{
	Handle(Prs3d_Drawer) style = parent->Context->HighlightStyle(Prs3d_TypeOfHighlight_None);
	parent->Context->SetAutomaticHilight(false);
	parent->Context->UnhilightSelected(TRUE);
	parent->Context->HilightSelected(TRUE);//去除已选中物体的高亮状态
	parent->Context->UpdateCurrentViewer();
}

