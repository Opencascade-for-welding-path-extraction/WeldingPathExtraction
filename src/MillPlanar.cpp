#include "MillPlanar.h"
#include "DisplayCore.h"
#include <list>
#include <opencascade/BRepBuilderAPI_MakeFace.hxx>
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/TopoDS_Face.hxx>
#include <opencascade/BRepOffsetAPI_MakeOffset.hxx>
#include <opencascade/TopoDS_Wire.hxx>
#include <opencascade/BRepBuilderAPI_MakeWire.hxx>
#include <opencascade/BRepBuilderAPI_MakeEdge.hxx>
#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/TopoDS_Face.hxx>
#include <opencascade/TopoDS_Edge.hxx>
#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/TopoDS_Wire.hxx>
#include <opencascade/TopoDS.hxx>
#include <opencascade/Standard_Handle.hxx>
#include <opencascade/StdSelect_BRepOwner.hxx>
#include <opencascade/BRep_Tool.hxx>
#include <opencascade/BRepAdaptor_CompCurve.hxx>
#include <opencascade/GCPnts_AbscissaPoint.hxx>
#include <opencascade/BRepAdaptor_Curve.hxx>
#include <opencascade/TopExp.hxx>
#include <opencascade/GC_MakeSegment.hxx>
#include <opencascade/Geom_Line.hxx>
#include <opencascade/Quantity_Color.hxx>
#include <opencascade/BRepGProp.hxx>
#include <opencascade/GProp_GProps.hxx>
#include <opencascade/GeomAPI_ExtremaCurveCurve.hxx>
#include <opencascade/Geom_Plane.hxx>
#include <opencascade/BRepAlgoAPI_Section.hxx>
#include <opencascade/Geom_Point.hxx>
#include <opencascade/BRepBuilderAPI_Transform.hxx>
#include <opencascade/BRep_Builder.hxx>
#include <opencascade/TopoDS_Compound.hxx>
#include <opencascade/Geom_CylindricalSurface.hxx>
#include <opencascade/gp.hxx>
#include <opencascade/gp_Lin2d.hxx>
#include <opencascade/GCE2d_MakeSegment.hxx>
#include <opencascade/BRepBuilderAPI_MakeEdge.hxx>
#include <opencascade/Geom2d_TrimmedCurve.hxx>
#include <opencascade/GeomAPI_IntCS.hxx>
#include <opencascade/BRepAdaptor_CompCurve.hxx>
#include <opencascade/Geom_BSplineCurve.hxx>
#include <opencascade/BRepExtrema_DistShapeShape.hxx>
#include <opencascade/BRepLib.hxx>




using namespace std;


CAM::MillPlanar::MillPlanar()
{


}

void CAM::MillPlanar::Init(MillPlanarGui* par1, PartSolution* par2)
{
	millPlanarGui = par1;
	viewerWindow = par2;
}

void CAM::MillPlanar::CreateToolPath()
{
	/*清除原有刀路*/
	//this->ClearToolPath();
	PathWireMap.clear();
	PathWireList.clear();
	TopoDS_Shape resultShape;
	/*生成轮廓铣刀路*/
	this->SetMillStrategy();
	/*定义wirePath复合体*/
	TopoDS_Compound millPathCompound;
	TopoDS_Compound movePathCompound;
	TopoDS_Compound	approchPathCompound;
	TopoDS_Compound	retractPathCompound;
	TopoDS_Compound	helicalPathCompound;
	BRep_Builder builder;
	builder.MakeCompound(millPathCompound);
	builder.MakeCompound(movePathCompound);
	builder.MakeCompound(approchPathCompound);
	builder.MakeCompound(retractPathCompound);
	builder.MakeCompound(helicalPathCompound);



	for (auto iter=PathWireMap.begin();iter!= PathWireMap.end();iter++)
	{
		/*进刀*/
		if (iter->first.find("Approach") != std::string::npos)
		{
			resultShape = this->ConvertACS(iter->second);
			builder.Add(approchPathCompound, resultShape);
			//viewerWindow->DisplayCoreManager->DisplayShape(resultShape, Aspect_TOL_SOLID,color,2.5, iter->first);

		}
		/*螺旋进刀*/
		if (iter->first.find("HelicalApproach") != std::string::npos)
		{
			resultShape = this->ConvertACS(iter->second);
			builder.Add(helicalPathCompound, resultShape);
			//viewerWindow->DisplayCoreManager->DisplayShape(resultShape, Aspect_TOL_SOLID,color,2.5, iter->first);

		}
		/*加工进给*/
		if (iter->first.find("Mill") != std::string::npos)
		{
			resultShape = this->ConvertACS(iter->second);
			builder.Add(millPathCompound, resultShape);
			//viewerWindow->DisplayCoreManager->DisplayShape(resultShape, Aspect_TOL_SOLID,color,2.5, iter->first);
			
		} 
		/*快移进给*/
		else if(iter->first.find("Move") != std::string::npos)
		{
			resultShape = this->ConvertACS(iter->second);
			builder.Add(movePathCompound, resultShape);
			//viewerWindow->DisplayCoreManager->DisplayShape(resultShape,Aspect_TOL_SOLID, color,2.5, iter->first);
		}
		/*退刀*/
		if (iter->first.find("Retract") != std::string::npos)
		{
			resultShape = this->ConvertACS(iter->second);
			builder.Add(retractPathCompound, resultShape);
			//viewerWindow->DisplayCoreManager->DisplayShape(resultShape, Aspect_TOL_SOLID,color,2.5, iter->first);

		}

	
	}

	/*显示直线进刀*/
	if (viewerWindow->DisplayCoreManager->ShapeManeger.count("ApproachPath") == 1)
	{
		viewerWindow->DisplayCoreManager->Context->Remove(viewerWindow->DisplayCoreManager->ShapeManeger["ApproachPath"]->AisShape, true);
		Quantity_Color color1(0 / 255.0, 0.0 / 255.0, 255.0 / 255.0, Quantity_TOC_RGB);
		viewerWindow->DisplayCoreManager->DisplayShape(approchPathCompound, Aspect_TOL_DASH, color1, 2.5, "ApproachPath");
	}
	else 
	{
		Quantity_Color color1(0/ 255.0, 0 / 255.0, 255.0 / 255.0, Quantity_TOC_RGB);
		viewerWindow->DisplayCoreManager->DisplayShape(approchPathCompound, Aspect_TOL_DASH, color1, 2.5, "ApproachPath");
	}

	/*显示螺旋进刀*/
	if (viewerWindow->DisplayCoreManager->ShapeManeger.count("HelicalApproach") == 1)
	{
		viewerWindow->DisplayCoreManager->Context->Remove(viewerWindow->DisplayCoreManager->ShapeManeger["HelicalApproach"]->AisShape, true);
		Quantity_Color color1(0 / 255.0, 0 / 255.0, 255.0 / 255.0, Quantity_TOC_RGB);
		viewerWindow->DisplayCoreManager->DisplayShape(helicalPathCompound, Aspect_TOL_DASH, color1, 2.5, "HelicalApproach");
	}
	else
	{
		Quantity_Color color1(0 / 255.0, 0 / 255.0,255.0 / 255.0, Quantity_TOC_RGB);
		viewerWindow->DisplayCoreManager->DisplayShape(approchPathCompound, Aspect_TOL_DASH, color1, 2.5, "HelicalApproach");
	}
	/*显示切削刀路*/
	if (viewerWindow->DisplayCoreManager->ShapeManeger.count("millpath") == 1)
	{
		viewerWindow->DisplayCoreManager->ShapeManeger["millpath"]->AisShape->SetShape(millPathCompound);
		viewerWindow->DisplayCoreManager->Context->Redisplay(viewerWindow->DisplayCoreManager->ShapeManeger["millpath"]->AisShape, true, true);
	}
	else
	{
		Quantity_Color color1(0, 255.0 / 255.0, 255.0 / 255.0, Quantity_TOC_RGB);
		viewerWindow->DisplayCoreManager->DisplayShape(millPathCompound, Aspect_TOL_SOLID, color1, 2.5, "millpath");
	}


	/*显示快速移动刀路*/
	if (viewerWindow->DisplayCoreManager->ShapeManeger.count("movepath") == 1)
	{
		viewerWindow->DisplayCoreManager->ShapeManeger["movepath"]->AisShape->SetShape(movePathCompound);
		viewerWindow->DisplayCoreManager->Context->Redisplay(viewerWindow->DisplayCoreManager->ShapeManeger["movepath"]->AisShape, true, true);
	}
	else
	{
		Quantity_Color color2(0, 255.0 / 255.0, 0, Quantity_TOC_RGB);
		viewerWindow->DisplayCoreManager->DisplayShape(movePathCompound, Aspect_TOL_SOLID, color2, 2.5, "movepath");
	}

	/*显示退刀*/
	if (viewerWindow->DisplayCoreManager->ShapeManeger.count("RetractPath") == 1)
	{
		viewerWindow->DisplayCoreManager->Context->Remove(viewerWindow->DisplayCoreManager->ShapeManeger["RetractPath"]->AisShape, true);
		Quantity_Color color1(0 / 255.0,0 / 255.0, 255.0 / 255.0, Quantity_TOC_RGB);
		viewerWindow->DisplayCoreManager->DisplayShape(retractPathCompound, Aspect_TOL_DASH, color1, 2.5, "RetractPath");
	}
	else
	{
		Quantity_Color color1(0/ 255.0, 0 / 255.0, 255.0 / 255.0, Quantity_TOC_RGB);
		viewerWindow->DisplayCoreManager->DisplayShape(retractPathCompound, Aspect_TOL_DASH, color1, 2.5, "RetractPath");
	}


	
}

void CAM::MillPlanar::CreateToolPath(Standard_Real Length, Standard_Real Width)
{
	list<TopoDS_Shape>shapeList;
	int  nr_of_counters{ 50 };
	double distance_between_contours{ 15 };
	// 定义矩形的四个顶点
	gp_Pnt p1(Length/2.0, Width/2.0, 0);
	gp_Pnt p2(Length / 2.0, -Width / 2.0, 0);
	gp_Pnt p3(-Length / 2.0, -Width / 2.0, 0);
	gp_Pnt p4(-Length / 2.0, Width / 2.0, 0);
	//make edge
	TopoDS_Edge aedge1 = BRepBuilderAPI_MakeEdge(p1, p2).Edge();
	TopoDS_Edge aedge2 = BRepBuilderAPI_MakeEdge(p2, p3).Edge();
	TopoDS_Edge aedge3 = BRepBuilderAPI_MakeEdge(p3, p4).Edge();
	TopoDS_Edge aedge4 = BRepBuilderAPI_MakeEdge(p4, p1).Edge();
	//Make wire
	TopoDS_Wire awire = BRepBuilderAPI_MakeWire(aedge1, aedge2, aedge3, aedge4).Wire();


	// 创建面
	TopoDS_Face face = BRepBuilderAPI_MakeFace(awire);
	BRepOffsetAPI_MakeOffset offset = BRepOffsetAPI_MakeOffset();
	offset.Init(GeomAbs_Arc);

	// 创建一个边缘的遍历器
	TopExp_Explorer wireExplorer;

	// 遍历面的线框
	for (wireExplorer.Init(face, TopAbs_WIRE); wireExplorer.More(); wireExplorer.Next())
	{
		TopoDS_Wire wire = TopoDS::Wire(wireExplorer.Current());
		offset.AddWire(wire);

	}

	for (int i = 0; i <= nr_of_counters; i++)
	{
		offset.Perform(-distance_between_contours * i);
		if (offset.IsDone())
		{
			shapeList.push_back(offset.Shape());


		}

	}
	Quantity_Color color(192.0 / 255.0, 192.0 / 255.0, 192 / 255.0, Quantity_TOC_RGB);
	viewerWindow->DisplayCoreManager->DisplayShape(TopoDS_Shape(face), color, 1);
	for (auto iter = shapeList.begin(); iter != shapeList.end(); iter++)
	{
		viewerWindow->DisplayCoreManager->DisplayShape(*iter, 0);
		Handle(AIS_Shape) ais_shape = new AIS_Shape(*iter);
		string pathwire = GetPathWireID();
		viewerWindow->DisplayCoreManager->ShapeManeger[pathwire] = new shape(ais_shape);
	}


}

void CAM::MillPlanar::GetShapeSurface()
{
	Handle(TopoDS_Face) selected__ais_face;
	viewerWindow->DisplayCoreManager->Context->InitDetected();
	Handle(Geom_Surface) aSurface;
	if (true)
	{
		Handle(AIS_InteractiveContext)& ctx = viewerWindow->DisplayCoreManager->Context;
		for (ctx->InitDetected(); ctx->MoreDetected(); ctx->NextDetected())
		{
			Handle(StdSelect_BRepOwner) owner = Handle(StdSelect_BRepOwner) ::DownCast(ctx->DetectedCurrentOwner());
			if (owner.IsNull())
				continue;

			TopoDS_Face highlightedFace = TopoDS::Face(owner->Shape());
			//aSurface = BRep_Tool::Surface(highlightedFace);
			millface = highlightedFace;
			break;
			

		}




	}
	
}

void CAM::MillPlanar::GetContinuousWire()
{
	list<TopoDS_Wire>wireList;
	/*赋值已经获取的高亮面*/
	TopoDS_Face face = millface;
	/*创建一个边缘的遍历器*/
	TopExp_Explorer wireExplorer;
	/*获取相连的Wire*/
	for (wireExplorer.Init(face, TopAbs_WIRE); wireExplorer.More(); wireExplorer.Next())
	{
		TopoDS_Wire wire = TopoDS::Wire(wireExplorer.Current());
		wireList.push_back(wire);
	}
	TopoDS_Wire firstWire;
	for (auto iter=wireList.begin();iter!=wireList.end();iter++)
	{
		if (*iter == wireList.front()) firstWire = *iter;

	}


}

void CAM::MillPlanar::IntelligentOffset()
{

}

void CAM::MillPlanar::ReciprocatingMill()
{
	list<TopoDS_Shape>shapeList;
	/*计算偏置轮廓数量*/
	int  offsetCounters{ 50 };
	/*获取刀具直径*/
	QString value = millPlanarGui->ComboBoxMap["刀具"]->currentText();
	Standard_Real toolDiameter = value.toDouble();
	/*初始化步距值*/
	Standard_Real toolSteep{ 0 };
	/*计算步距*/
	toolSteep = toolDiameter * millPlanarGui->LineEditMap["平面直径百分比"]->text().toDouble() / 100.0;
	/*获取要铣削平面*/
	TopoDS_Face face = millface;
	// 创建转换操作
	TopoDS_Shape resultshape=this->ConvertWCS(face);
	face = TopoDS::Face(resultshape);
	//viewerWindow->DisplayCoreManager->DisplayShape(face, 1);
	resultshape = this->ConvertACS(face);
	//viewerWindow->DisplayCoreManager->DisplayShape(resultshape, 1);
	/*获取要铣削平面的Z项高度*/
	Handle(Geom_Surface) aSurface = BRep_Tool::Surface(face);
	Standard_Real domainU{ 0 }, domainV{0};
	gp_Pnt point;
	point = aSurface->Value(domainU, domainV);
	Standard_Real Z = point.Z();
	/*遍历获取面的所有端点*/

	// 创建一个边缘的遍历器
	TopExp_Explorer wireExplorer;
	TopExp_Explorer edgeExplorer;

	list<Standard_Real>xValuesList;//排序前的列表
	list<Standard_Real>yValuesList;//排序前的列表
	list<Standard_Real>__xValuesList;//排序后的列表
	list<Standard_Real>__yValuesList;//排序后的列表
	list<Standard_Real>::iterator itr;
	
	for (wireExplorer.Init(face, TopAbs_WIRE); wireExplorer.More(); wireExplorer.Next())
	{
		TopoDS_Wire wire = TopoDS::Wire(wireExplorer.Current());
		BRepAdaptor_CompCurve curve(wire);
		for (edgeExplorer.Init(wire, TopAbs_EDGE); edgeExplorer.More(); edgeExplorer.Next())
		{
			const TopoDS_Edge& edge= TopoDS::Edge(edgeExplorer.Current());
			BRepAdaptor_Curve curve(edge);
			auto FirstV = TopExp::FirstVertex(edge);
			auto LastV = TopExp::LastVertex(edge);
			xValuesList.push_back(BRep_Tool::Pnt(FirstV).X());
			yValuesList.push_back(BRep_Tool::Pnt(FirstV).Y());
			xValuesList.push_back(BRep_Tool::Pnt(LastV).X());
			yValuesList.push_back(BRep_Tool::Pnt(LastV).Y());
			
		}
	}
	xValuesList.sort();
	yValuesList.sort();
	/*排序*/
	for (itr=xValuesList.begin(); itr != xValuesList.end(); ++itr)
		__xValuesList.push_back(*itr);
	for (itr = yValuesList.begin(); itr != yValuesList.end(); ++itr)
		__yValuesList.push_back(*itr);

	Standard_Real startX,startY,endX,endY,maxX, maxY, minX, minY;
	maxX = __xValuesList.back()+20;
	minX = __xValuesList.front()-20;
	maxY = __yValuesList.back()+20;
	minY = __yValuesList.front()-20;
	/*生成路径*/
	startX = minX;
	startY = maxY;
	endX = maxX;
	endY = maxY;
	GC_MakeSegment *aSegment;
	TopoDS_Edge anEdge;
	TopoDS_Wire awire;
	int direction = 1;
	int ID = 0;
	string FeedMode{};
	while (startY >minY)
	{
		if (ID!=0 )
		{
			/*生成横向路径*/
			//endX = maxX;
			aSegment = new GC_MakeSegment(gp_Pnt(startX, startY, Z), gp_Pnt(endX, endY, Z));
			anEdge = BRepBuilderAPI_MakeEdge(aSegment->Value());
			awire = BRepBuilderAPI_MakeWire(anEdge);
			PathWireList.push_back(awire);
			FeedMode = "Mill" + to_string(ID);
			PathWireMap[FeedMode] = awire;
			/*生成纵向路径*/
			startX = endX;
			startY = endY;
			endY -= toolSteep;
			aSegment = new GC_MakeSegment(gp_Pnt(startX, startY, Z), gp_Pnt(endX, endY, Z));
			anEdge = BRepBuilderAPI_MakeEdge(aSegment->Value());
			awire = BRepBuilderAPI_MakeWire(anEdge);
			PathWireList.push_back(awire);
			FeedMode = "Move" + to_string(ID);
			PathWireMap[FeedMode] = awire;
			/*退刀点*/
			endPoint = gp_Pnt(endX, endY, Z);
		}
		else
		{
			/*生成纵向路径*/
			aSegment = new GC_MakeSegment(gp_Pnt(startX, startY + toolSteep, Z), gp_Pnt(startX, startY, Z));
			anEdge = BRepBuilderAPI_MakeEdge(aSegment->Value());
			awire = BRepBuilderAPI_MakeWire(anEdge);
			PathWireList.push_back(awire);
			FeedMode = "Move" + to_string(ID);
			PathWireMap[FeedMode] = awire;
			/*进刀点*/
			startPoint = gp_Pnt(startX, startY + toolSteep, Z);
			ID++;
			continue;
		}
		

		ID++;
		if (direction==1)
		{
			startX = maxX;
			startY = endY;
			endX = minX;
			direction = -1;
		}
		else if(direction==-1)
		{
			startX = minX;
			startY = endY;
			endX = maxX;
			direction = 1;
		}
		
	}
	AddLinerApproach();
	AddRetract();
	
}

void CAM::MillPlanar::ContourFollowingMill()
{
	list<TopoDS_Shape>shapeList;
	/*计算偏置轮廓数量*/
	int  offsetCounters{50};
	/*获取刀具直径*/
	QString value = millPlanarGui->ComboBoxMap["刀具"]->currentText();
	Standard_Real toolDiameter= value.toDouble();
	/*初始化步距值*/
	Standard_Real toolSteep{0};
	/*计算步距*/
	toolSteep = toolDiameter * millPlanarGui->LineEditMap["平面直径百分比"]->text().toDouble()/100.0;


	//TopoDS_Face face = millface;
	TopoDS_Shape resultShape = this->ConvertWCS(millface);
	TopoDS_Face face = TopoDS::Face(resultShape);
	BRepOffsetAPI_MakeOffset offset = BRepOffsetAPI_MakeOffset();
	offset.Init(GeomAbs_Arc);

	// 创建一个边缘的遍历器
	TopExp_Explorer wireExplorer;

	
	Standard_Real wireMaxLength{ 0 };
	TopoDS_Wire maxLengthWire;
	/*遍历面的线框*/
	for (wireExplorer.Init(face, TopAbs_WIRE); wireExplorer.More(); wireExplorer.Next())
	{
		TopoDS_Wire wire = TopoDS::Wire(wireExplorer.Current());
		BRepAdaptor_CompCurve curve(wire);
		if (GCPnts_AbscissaPoint::Length(curve) > wireMaxLength)
		{
			wireMaxLength = GCPnts_AbscissaPoint::Length(curve);
			maxLengthWire = wire;
		}

	}
	/*将要偏置的轮廓添加到偏置器里面*/
	offset.AddWire(maxLengthWire);

	/*循环生成偏置轮廓*/
	int ID = 0;
	string FeedMode{};
	GProp_GProps gProps;
	for (int i = 0; i <= offsetCounters; i++)
	{
		offset.Perform(-toolSteep * i);
		if (offset.IsDone())
		{
			// 计算线性属性（包括长度）
			BRepGProp::LinearProperties(offset.Shape(), gProps);
			Standard_Real lengeh = gProps.Mass();
			if (lengeh <= 0) break;
			shapeList.push_back(offset.Shape());
			FeedMode ="Mill"+to_string(ID);
			PathWireMap[FeedMode] = offset.Shape();
			ID++;
			
		
		}
	}
	/*获取最后一个偏置轮廓的几何中心*/
	TopoDS_Wire awire = TopoDS::Wire(shapeList.back());
	TopoDS_Face aface = BRepBuilderAPI_MakeFace(awire).Face();
	
	/*获取质心点*/
	gProps.CentreOfMass();
	/*计算面的属性（面积、质心等）*/
	BRepGProp::SurfaceProperties(aface, gProps);
	/*设置进刀点 最后刀轨几何中心*/
	startPoint = gProps.CentreOfMass();
	/*设置退刀点 最后刀轨几何中心*/
	endPoint= gProps.CentreOfMass();
	 /*获取从最外侧进刀的进刀点*/
	 Handle(Geom_Line) geomLine1 = new Geom_Line(startPoint,gp_Dir(0,1,0));
	 TopoDS_Edge aline = BRepBuilderAPI_MakeEdge(geomLine1);
	 for (TopExp_Explorer explorer(maxLengthWire, TopAbs_EDGE); explorer.More(); explorer.Next())
	 {
		 const TopoDS_Edge& edge = TopoDS::Edge(explorer.Current());
		 // 提取几何曲线
		 Standard_Real first, last;
		 Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, first, last);
		 BRepExtrema_DistShapeShape Extrema= BRepExtrema_DistShapeShape(edge,aline);
		 gp_Pnt point1 = Extrema.PointOnShape1(1);
		 gp_Pnt point2 = Extrema.PointOnShape2(1);
		 qDebug() << point1.Distance(point2);
		 if (point1.Distance(point2)<=0.01)
		 {
			 startPoint = Extrema.PointOnShape1(1);
			 break;
		 }
		 
	 }
	 // 创建交点计算器
	
	/*赋值给类内路径变量*/
	PathWireList = shapeList;
	/*螺旋进刀*/
	AddHelicalApproach();
	/*直线进刀*/
	AddLinerApproach();
	/*退刀*/
	AddRetract();
}

/*轮廓加工 手动选取加工轮廓线*/
void CAM::MillPlanar::ContourMill()
{
	TopoDS_Wire awire;
	TopoDS_Edge aedge;
	BRepBuilderAPI_MakeWire pathWire;
	int ID {0};
	string FeedMode{};
	auto selectCurvesList = viewerWindow->interactivemanager->selectedShapesList;
	for (auto &iter:selectCurvesList)
	{
		aedge = TopoDS::Edge(iter);
		awire = BRepBuilderAPI_MakeWire(aedge);
		ID = PathWireList.size();
		PathWireList.push_back(awire);
		FeedMode = "Mill" + to_string(ID);
		PathWireMap[FeedMode] = awire;
		/*生成相连的wire*/
		pathWire.Add(aedge);
	}
	/*将相连wire转变成edge*/
	TopoDS_Vertex startVertex, endVertex;
	TopTools_IndexedMapOfShape vertexMap;
	/*获取路径的所有顶点 没有重复*/
	TopExp::MapShapes(pathWire, TopAbs_VERTEX, vertexMap);
	vector<TopoDS_Vertex>vertexList,copy_vertexList,same_verteList;
	/*获取路径的所有顶点*/
	gp_Pnt point1, point2;
	for (int i = 1; i <= vertexMap.Extent(); ++i) {
		const TopoDS_Shape& vertexShape = vertexMap.FindKey(i);
		TopoDS_Vertex vertex = TopoDS::Vertex(vertexShape);
		if (i == 1)
		{
			 startPoint = BRep_Tool::Pnt(vertex);
		}
		else if (i == vertexMap.Extent())
		{
			endPoint= BRep_Tool::Pnt(vertex);
		}

	}
	AddLinerApproach();
	AddRetract();
	viewerWindow->interactivemanager->ClearHilightObjects();
}

void CAM::MillPlanar::SetMillStrategy()
{
	auto mode = millPlanarGui->ComboBoxMap["切削模式"]->currentIndex();
	
	if (mode==0)
	{
		this->ReciprocatingMill();
	}
	else if (mode == 1)
	{
		this->ContourFollowingMill();
	}
	else if (mode == 2)
	{
		this->ContourMill();
		
	}
}

void CAM::MillPlanar::ClearToolPath()
{
	/*移除已经显示的进刀*/
	if (viewerWindow->DisplayCoreManager->ShapeManeger.count("ApproachPath") == 1)
	{
		viewerWindow->DisplayCoreManager->Context->Remove(viewerWindow->DisplayCoreManager->ShapeManeger["ApproachPath"]->AisShape, true);
		viewerWindow->DisplayCoreManager->ShapeManeger.erase("ApproachPath");
	}
	/*移除已经显示的螺旋进刀*/
	if (viewerWindow->DisplayCoreManager->ShapeManeger.count("HelicalApproach") == 1)
	{
		viewerWindow->DisplayCoreManager->Context->Remove(viewerWindow->DisplayCoreManager->ShapeManeger["HelicalApproach"]->AisShape, true);
		viewerWindow->DisplayCoreManager->ShapeManeger.erase("HelicalApproach");
	}
	/*移除已经显示的切削刀路*/
	if (viewerWindow->DisplayCoreManager->ShapeManeger.count("millpath") == 1)
	{
		viewerWindow->DisplayCoreManager->Context->Remove(viewerWindow->DisplayCoreManager->ShapeManeger["millpath"]->AisShape, true);
		viewerWindow->DisplayCoreManager->ShapeManeger.erase("millpath");
	}

	/*移除已经显示的快移刀路*/
	if (viewerWindow->DisplayCoreManager->ShapeManeger.count("movepath") == 1)
	{
		viewerWindow->DisplayCoreManager->Context->Remove(viewerWindow->DisplayCoreManager->ShapeManeger["movepath"]->AisShape, true);
		viewerWindow->DisplayCoreManager->ShapeManeger.erase("movepath");
	}
	/*移除已经显示的退刀刀路*/
	if (viewerWindow->DisplayCoreManager->ShapeManeger.count("RetractPath") == 1)
	{
		viewerWindow->DisplayCoreManager->Context->Remove(viewerWindow->DisplayCoreManager->ShapeManeger["RetractPath"]->AisShape, true);
		viewerWindow->DisplayCoreManager->ShapeManeger.erase("RetractPath");
	}
	
	PathWireMap.clear();
	PathWireList.clear();
	viewerWindow->DisplayCoreManager->Context->UpdateCurrentViewer();
	
}

string CAM::MillPlanar::GetPathWireID()
{
	PathWireID++;
	string pathWireid ="PathWire"+ std::to_string(PathWireID);
	return pathWireid;
}

void CAM::MillPlanar::CreateWCS()
{
	

	if (viewerWindow->DisplayCoreManager->ShapeManeger.count("工作坐标系")==1)
	{
		auto aisrihedron = viewerWindow->DisplayCoreManager->ShapeManeger["工作坐标系"]->Aisrihedron;
		viewerWindow->DisplayCoreManager->Context->Remove(aisrihedron, Standard_True);
		viewerWindow->DisplayCoreManager->ShapeManeger.erase("工作坐标系");
	}


	if (directionXCreated == 1 and directionYCreated == 1 and originPointCreated == 1)
	{
		try
		{
			viewerWindow->DisplayCoreManager->DisplaytWCSriehedron(originPoint, directionX, directionY);
			gp_Dir directionZ = directionX.Crossed(directionY);
			WcsgpAx3 = gp_Ax3(originPoint, directionZ, directionX);
		}
		catch (...)
		{
			qDebug() << "CreateWCS error";

		}
	
		
	}
	
}

void CAM::MillPlanar::CreateoriginPoint()
{
	if (!viewerWindow->interactivemanager->captureHilightPoint.IsNull())
	{
		originPoint = viewerWindow->interactivemanager->captureHilightPoint->Component()->Pnt();
		originPointCreated = 1;
	}
}

void CAM::MillPlanar::CreateVector(Vector_Type thetype, Standard_Integer reversed)
{
	TopoDS_Shape VectorShape;

	if (reversed != -1 )
	{
		if (!viewerWindow->interactivemanager->selectedShape.IsNull())
		{
			VectorShape = viewerWindow->interactivemanager->selectedShape;
		}
		else
		{
			if (thetype == X_Vector)
			{

				if (!xVectorShape.IsNull())
					VectorShape = TopoDS_Shape(xVectorShape);
			}
			else if (thetype == Y_Vector)
			{
				if (!yVectorShape.IsNull())
					VectorShape = TopoDS_Shape(yVectorShape);
			}
		}
	
		
	}

	else if (reversed == -1)
	{
		if (thetype == X_Vector)
		{
		
			if(!xVectorShape.IsNull())
				VectorShape = TopoDS_Shape(xVectorShape);
		}
		else if(thetype == Y_Vector)
		{
			if (!yVectorShape.IsNull())
				VectorShape = TopoDS_Shape(yVectorShape);
		}

	}
   if (!VectorShape.IsNull() and VectorShape.ShapeType()== TopAbs_ShapeEnum::TopAbs_EDGE)
   {
	   gp_Dir direction;
	   TopoDS_Edge edge = TopoDS::Edge(VectorShape);
	   Standard_Real firstParam, lastParam;
	   Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, firstParam, lastParam);
	   /*检查曲线是否是 Geom_Line 类型*/
	   Handle(Geom_Line) geomLine = Handle(Geom_Line)::DownCast(curve);
	   if (!geomLine.IsNull()) 
	   {
		   direction=geomLine->Lin().Direction();
		 
		   /*获取edge 端点*/
		   auto FirstV = TopExp::FirstVertex(edge);
		   auto LastV = TopExp::LastVertex(edge);

		   /*端点1 gp_pnt*/
		   gp_Pnt point1 = BRep_Tool::Pnt(FirstV);
		   /*端点2 gp_pnt*/
		   gp_Pnt point2 = BRep_Tool::Pnt(LastV);
		   /*端点 gp_pnt*/
		   gp_Pnt point=point1;

		   /*反向向操作*/
		   if (reversed == -1)
		   {
			   point = point2;

		   }

		   /*获取朝外的方向*/
		   BRepBuilderAPI_MakeEdge aedge = BRepBuilderAPI_MakeEdge(gp_Lin(point, direction), 0, 10000.0);
		   Handle(Geom_Curve) __curve = BRep_Tool::Curve(aedge, firstParam, lastParam);
		   GeomAPI_ExtremaCurveCurve intersector(curve, __curve);

		   qDebug() << intersector.NbExtrema();
		   if (reversed!=-1)
		   {
			   if (intersector.NbExtrema() > 1)
			   {

				   Quantity_Color color = Quantity_Color(0 / 255.0, 0 / 255.0, 0 / 255.0, Quantity_TOC_RGB);
				   viewerWindow->DisplayCoreManager->DisplayAxis(thetype, point, direction, color);
			   }
			   else if (intersector.NbExtrema() == 1)
			   {
				   Quantity_Color color = Quantity_Color(0 / 255.0, 0 / 255.0, 0 / 255.0, Quantity_TOC_RGB);
				   direction = direction.Reversed();
				   viewerWindow->DisplayCoreManager->DisplayAxis(thetype, point, direction, color);
			   }
		   } 
		   else if (reversed == -1)
		   {
			   if (intersector.NbExtrema() > 1)
			   {

				   Quantity_Color color = Quantity_Color(0 / 255.0, 0 / 255.0, 0 / 255.0, Quantity_TOC_RGB);
				   direction = direction.Reversed();
				   viewerWindow->DisplayCoreManager->DisplayAxis(thetype, point, direction, color);
			   }
			   else if (intersector.NbExtrema() == 1)
			   {
				   Quantity_Color color = Quantity_Color(0 / 255.0, 0 / 255.0, 0 / 255.0, Quantity_TOC_RGB);
				   viewerWindow->DisplayCoreManager->DisplayAxis(thetype, point, direction, color);
			   }
		   }
		  

		   /*矢量赋值*/

		   if (thetype == 0)
		   {
			   directionX=direction;
			   VectorXDisPlayed = 1;
			   directionXCreated = 1;
			   xVectorShape = VectorShape;
		   }
		   else if (thetype == 1)
		   {
			   directionY = direction;
			   VectorYDisPlayed = 1;
			   directionYCreated = 1;
			   yVectorShape = VectorShape;
		   }
		  
		   

	   }
   }
   else if (!VectorShape.IsNull() and VectorShape.ShapeType() == TopAbs_ShapeEnum::TopAbs_FACE)
   {
	   gp_Dir direction;
	   Handle(Geom_Surface) aSurface;
	   const TopoDS_Face& face = TopoDS::Face(VectorShape);
	   aSurface = BRep_Tool::Surface(face);
	   Standard_Real domainU, domainV = 0;
	   gp_Pnt point;
	   gp_Vec U1;
	   gp_Vec V1;
	   /*获取面的法线方向*/
	   aSurface->D1(0, 0, point, U1, V1);
	   gp_Vec direct = U1.Crossed(V1);
	   auto x = direct.X();
	   auto y = direct.Y();
	   auto z = direct.Z();
	   auto coord = direct.Coord(3);
	   try
	   {
		   direction = gp_Dir(direct);
		  
	   }
	   catch (...)
	   {
		   qDebug() << "Error";
	   }

	   GProp_GProps gprops;

	   /*计算面的属性（面积、质心等）*/
	   BRepGProp::SurfaceProperties(face, gprops);
	   /*获取质心点*/
	   gprops.CentreOfMass();
	   /*计算面的属性（面积、质心等）*/
	   BRepGProp::SurfaceProperties(face, gprops);
	   point= gprops.CentreOfMass();
	   /*生成偏移的基准平面*/
	   map<string, shape*> shapemap = viewerWindow->DisplayCoreManager->ShapeManeger;

	   for (auto iter = shapemap.begin(); iter != shapemap.end(); iter++)
	   {
		   if (!iter->second->AisShape.IsNull())
		   {
			   qDebug() << iter->second->AisShape->Shape().ShapeType();
			   /*创建几何平面*/
			   Handle(Geom_Plane) plane = new Geom_Plane(point, direction);
			   /*偏移创建的几何平面*/
			   gp_Dir normal = plane->Pln().Axis().Direction();
			   /*获取平面的原始参考点*/
			   gp_Pnt origin = plane->Location();
			   /*计算新的参考点：在原始点沿法向量方向偏移*/
			   gp_Vec offsetVec(normal);  // 将法向量转换为向量
			   offsetVec *= 2.0;  // 乘以偏移距离
			   gp_Pnt newOrigin = point.Translated(offsetVec);  // 偏移参考点
			   // 创建新的平面
			   Handle(Geom_Plane) offsetPlane = new Geom_Plane(newOrigin, normal);
			   BRepAlgoAPI_Section sectionAlgo;
			   sectionAlgo.Init1(iter->second->AisShape->Shape()); // 初始化第一个形状
			   sectionAlgo.Init2(offsetPlane); // 初始化第二个形状（平面）
			   sectionAlgo.Build();      // 计算交线
			   TopoDS_Shape result = sectionAlgo.Shape();
			   TopTools_ListOfShape edgelist = sectionAlgo.SectionEdges();//交线列表
			   // 检查是否成功构建交线
			   if (reversed!=-1)
			   {
				   if (edgelist.Size() == 0)
				   {
					   Quantity_Color color = Quantity_Color(0 / 255.0, 0 / 255.0, 0 / 225.0, Quantity_TOC_RGB);
					   viewerWindow->DisplayCoreManager->DisplayAxis(thetype, point, direction, color);
				   }
				   else
				   {
					   Quantity_Color color = Quantity_Color(0 / 255.0, 0 / 255.0, 0 / 225.0, Quantity_TOC_RGB);
					   direction = direction.Reversed();
					   viewerWindow->DisplayCoreManager->DisplayAxis(thetype, point, direction, color);
				   }

			   }
			   else if (reversed==-1)
			   {

				   if (edgelist.Size() == 0)
				   {
					   Quantity_Color color = Quantity_Color(0 / 255.0, 0 / 255.0, 0 / 225.0, Quantity_TOC_RGB);
					   direction = direction.Reversed();
					   viewerWindow->DisplayCoreManager->DisplayAxis(thetype, point, direction, color);
				   }
				   else
				   {
					   Quantity_Color color = Quantity_Color(0 / 255.0, 0 / 255.0, 0 / 225.0, Quantity_TOC_RGB);
					   viewerWindow->DisplayCoreManager->DisplayAxis(thetype, point, direction, color);
				   }
			   }

			 

			   if (thetype == X_Vector)
			   {

				   directionX = direction;
				   xVectorShape = TopoDS_Shape(VectorShape);
				   VectorXDisPlayed = 1;
				   directionXCreated = 1;
			   }
			   else if (thetype == Y_Vector)
			   {
				   directionY = direction;
				   yVectorShape = TopoDS_Shape(VectorShape);
				   VectorYDisPlayed = 1;
				   directionYCreated = 1;
			   }

		   }
		   else
		   {
			   //qDebug() << "SUCCEED";
		   }

	   }

	   
	   
   }
   /*清空当前的selectedShape*/
  viewerWindow->interactivemanager ->selectedShape.Nullify();
}

void CAM::MillPlanar::ReverseVector(Vector_Type thetype, Standard_Integer thereversed)
{
	if (thetype==0)
	{
		if (this->VectorXDisPlayed != 0)
		{
			viewerWindow->DisplayCoreManager->RemoveAxis(X_Vector);
			this->VectorXDisPlayed = 0;
		}

		CreateVector(thetype, thereversed);

	} 
	else if((thetype == 1))
	{

		if (this->VectorYDisPlayed != 0)
		{
			viewerWindow->DisplayCoreManager->RemoveAxis(Y_Vector);
			this->VectorYDisPlayed = 0;
		}

		CreateVector(thetype, thereversed);

	}
	this->CreateWCS();
}

void CAM::MillPlanar::ParaInit()
{
	originPointCreated = 0;
	directionXCreated = 0;
	directionYCreated = 0;
	VectorXDisPlayed = 0;
	VectorYDisPlayed = 0;
}

void CAM::MillPlanar::RemoveAllVector()
{
	if (this->VectorXDisPlayed != 0)
	{
		viewerWindow->DisplayCoreManager->RemoveAxis(X_Vector);
		this->VectorXDisPlayed = 0;
	}

	if (this->VectorYDisPlayed != 0)
	{
		viewerWindow->DisplayCoreManager->RemoveAxis(Y_Vector);
		this->VectorYDisPlayed = 0;
	}
}

TopoDS_Shape CAM::MillPlanar::ConvertWCS(const TopoDS_Shape theshape)
{
	// 定义一个 gp_Trsf 对象，用来存储转换矩阵
	gp_Trsf transformation;
	TopoDS_Shape resultShape;
	transformation.SetTransformation(WcsgpAx3);
	BRepBuilderAPI_Transform transform(theshape, transformation);
	// 获取变换后的形状
	resultShape = transform.Shape();
	return TopoDS_Shape(resultShape);
}

TopoDS_Shape CAM::MillPlanar::ConvertACS(const TopoDS_Shape theshape)
{
	// 定义一个 gp_Trsf 对象，用来存储转换矩阵
	gp_Trsf transformation;
	TopoDS_Shape resultShape;
	transformation.SetTransformation(WcsgpAx3, gp::XOY());
	BRepBuilderAPI_Transform transform(theshape, transformation);
	// 获取变换后的形状
	resultShape = transform.Shape();
	return TopoDS_Shape(resultShape);
}

bool CAM::MillPlanar::OutPutCLSFile()
{
	return true;
}

void CAM::MillPlanar::CreateCLSF()
{

}

void CAM::MillPlanar::AddHelicalApproach()
{
	string FeedMode;
	Standard_Real Helix_r = 3;
	Standard_Real a = 2.0;
	Standard_Real b = 2.0;
	Standard_Real c = 10;
	Standard_Real d =10;
	Standard_Real vx,vy,vz;
	gp_Ax3 location= gp_Ax3(startPoint,gp_Dir(0,0,-1));
	Handle(Geom_CylindricalSurface) aCylinder =new  Geom_CylindricalSurface(location, Helix_r);//生成圆柱面（螺旋形b半径）
	gp_Lin2d aLine2d = gp_Lin2d(gp_Pnt2d(startPoint.X(), startPoint.Y()), gp_Dir2d(a, b));//斜率COS(X) = a / b
	Handle(Geom2d_TrimmedCurve) aSegment = GCE2d_MakeSegment(aLine2d, 0.0, M_PI * c).Value();
	TopoDS_Edge  helix_edge = BRepBuilderAPI_MakeEdge(aSegment->BasisCurve(), aCylinder, 0,d* M_PI).Edge();//d * pi为螺线线长度 d为圈数
	//TopoDS_Wire  helix_wire = TopoDS::Wire(helix_edge);
	TopoDS_Vertex startVertex, endVertex;
	TopTools_IndexedMapOfShape vertexMap;
	/*获取螺旋线端点*/
	TopExp::MapShapes(helix_edge, TopAbs_VERTEX, vertexMap);
	/*定义螺旋线两个端点*/
	gp_Pnt point1, point2;
	/*获取螺旋线两个端点*/
	for (int i = 1; i <= vertexMap.Extent(); ++i) {
		const TopoDS_Shape& vertexShape = vertexMap.FindKey(i);
		TopoDS_Vertex vertex = TopoDS::Vertex(vertexShape);
		if (i == 1)
		{
			point1 = BRep_Tool::Pnt(vertex);
		}
		else if (i == vertexMap.Extent())
		{
			point2 = BRep_Tool::Pnt(vertex);
		}

	}
	gp_Trsf transform;
	/*设置平移的向量(vx, vy, vz)*/
	gp_Vec translation;
	if (point1.Z() > point2.Z())
	{
		vx = startPoint.X()- point2.X();
		vy =startPoint.Y()- point2.Y();
		vz = -point2.Z() + startPoint.Z();
		translation= gp_Vec(vx, vy, vz);
	}
	else
	{
		vx = startPoint.X()- point1.X() ;
		vy = startPoint.Y()-point1.Y() ;
		vz = -point1.Z() + startPoint.Z();
		translation = gp_Vec(vx, vy, vz);
	}
	
	/*设置平移向量*/
	transform.SetTranslation(translation);
	/*应用变换到形状*/
	BRepBuilderAPI_Transform transformer(helix_edge, transform);
	/*/获取变换后的新形状*/
	helix_edge = TopoDS::Edge(transformer.Shape());
	PathWireList.push_back(helix_edge);
	FeedMode = "HelicalApproach";
	PathWireMap[FeedMode] = helix_edge;
}

void CAM::MillPlanar::AddLinerApproach()
{
	/*获取第一个快速移动路径起点*/
	Standard_Real point[3];
	/*抬刀高度*/
	Standard_Real height{20};
	string FeedMode;
	point[0] = startPoint.X();
	point[1] = startPoint.Y();
	point[2] = startPoint.Z();
	GC_MakeSegment* aSegment = new GC_MakeSegment(gp_Pnt(point[0], point[1], point[2]), gp_Pnt(point[0], point[1], point[2]+height));
	TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aSegment->Value());
	TopoDS_Wire awire = BRepBuilderAPI_MakeWire(anEdge);
	PathWireList.push_back(awire);
	FeedMode = "ApproachPath" ;
	PathWireMap[FeedMode] = awire;

}

void CAM::MillPlanar::AddRetract()
{

	/*获取第一个快速移动路径起点*/
	Standard_Real point[3];
	/*抬刀高度*/
	Standard_Real height{ 20 };
	string FeedMode;
	point[0] = endPoint.X();
	point[1] = endPoint.Y();
	point[2] = endPoint.Z();
	GC_MakeSegment* aSegment = new GC_MakeSegment(gp_Pnt(point[0], point[1], point[2]), gp_Pnt(point[0], point[1], point[2] + height));
	TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aSegment->Value());
	TopoDS_Wire awire = BRepBuilderAPI_MakeWire(anEdge);
	PathWireList.push_back(awire);
	FeedMode = "RetractPath";
	PathWireMap[FeedMode] = awire;
}

void CAM::MillPlanar::GetWeldingWire()
{
	list<TopoDS_Shape>shapeList;
	/*计算偏置轮廓数量*/
	int  offsetCounters{ 50 };
	auto shape = viewerWindow->DisplayCoreManager->currentShape;
	// 创建一个边缘的遍历器
	TopExp_Explorer faceExplorer;
	GProp_GProps gProps;
	for (TopExp_Explorer faceExp(shape, TopAbs_FACE); faceExp.More(); faceExp.Next())
	{
		const TopoDS_Face& face = TopoDS::Face(faceExp.Current());
		//std::cout << "Face found!" << std::endl;
		// 遍历当前面上的所有边 (TopAbs_EDGE)
		for (TopExp_Explorer edgeExp(face, TopAbs_EDGE); edgeExp.More(); edgeExp.Next())
		{
			const TopoDS_Edge& edge = TopoDS::Edge(edgeExp.Current());
			//std::cout << "  Edge found!" << std::endl;
			// 计算线性属性（包括长度）
			BRepGProp::LinearProperties(edge, gProps);
			Standard_Real lengeh = gProps.Mass();
			BRepAdaptor_Curve curve(edge);
			auto FirstV = TopExp::FirstVertex(edge);
			auto LastV = TopExp::LastVertex(edge);
			//qDebug() << lengeh ;
			/*简单设置筛选条件*/
			if (lengeh == 400 and BRep_Tool::Pnt(FirstV).Y() == 12.0 and BRep_Tool::Pnt(LastV).Y() == 12.0)
			{
				Quantity_Color color1(0 / 255.0, 0.0 / 255.0, 255.0 / 255.0, Quantity_TOC_RGB);
				viewerWindow->DisplayCoreManager->DisplayShape(edge, Aspect_TOL_DASH, color1, 2.5, "ApproachPath");
			}
			else if (lengeh == 12 and BRep_Tool::Pnt(FirstV).Y() == 12.0 and BRep_Tool::Pnt(LastV).Y() == 12.0)
			{
				Quantity_Color color1(0 / 255.0, 0.0 / 255.0, 255.0 / 255.0, Quantity_TOC_RGB);
				viewerWindow->DisplayCoreManager->DisplayShape(edge, Aspect_TOL_DASH, color1, 2.5, "ApproachPath");
			}
			else if (lengeh == 300 and BRep_Tool::Pnt(FirstV).Y() == 0.0 and BRep_Tool::Pnt(LastV).Y() == 0.0)
			{
				Quantity_Color color1(0 / 255.0, 0.0 / 255.0, 255.0 / 255.0, Quantity_TOC_RGB);
				viewerWindow->DisplayCoreManager->DisplayShape(edge, Aspect_TOL_DASH, color1, 2.5, "ApproachPath");
			}

		}
	}
}




