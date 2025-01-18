#include "DisplayCore.h"
#include "InteractiveModule.h"
#include "OccView.h"
#include <iostream>
#include <opencascade/AIS_InteractiveContext.hxx>
#include <opencascade/AIS_Shape.hxx>
#include <opencascade/AIS_Trihedron.hxx>
#include <opencascade/AIS_Line.hxx>
#include <opencascade/Aspect_TypeOfLine.hxx>
#include <opencascade/BRepPrimAPI_MakeBox.hxx>
#include <opencascade/BRepTools.hxx> 
#include <opencascade/Geom_Axis2Placement.hxx>
#include <opencascade/Geom_Plane.hxx>
#include <opencascade/gp.hxx>
#include <opencascade/Graphic3d_TransModeFlags.hxx>
#include <opencascade/Prs3d_LineAspect.hxx>
#include <opencascade/Prs3d_LineAspect.hxx>
#include <opencascade/Prs3d_PlaneAspect.hxx>
#include <opencascade/Quantity_Color.hxx>
#include <opencascade/Quantity_Color.hxx>
#include <opencascade/STEPCAFControl_Reader.hxx>
#include <opencascade/STEPControl_Reader.hxx> 
#include <opencascade/STEPControl_Reader.hxx>
#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/TopoDS.hxx>
#include <opencascade/TopoDS_Face.hxx>
#include <opencascade/TopoDS_Shape.hxx> 
#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/Geom_Line.hxx>
#include <opencascade/Graphic3d_TransformPers.hxx>
#include <opencascade/AIS_Axis.hxx>
#include <QFileDialog>
#include <qmessagebox.h>
#include<string.h>
#include <QDebug>

using namespace std;
using namespace CAM;


CAM::DisplayCore::DisplayCore(OccView*parent)
{
	Context =parent->getContext();
	myOccView = parent;
	//DisplayCube();
	Displaytriehedron();
	//DisplayPlane();

}
void CAM::DisplayCore::DisplayShape(TopoDS_Shape shape, Quantity_Color color, double transparency=0, Graphic3d_MaterialAspect material, Standard_Boolean theToUpdateViewer)
{
	Handle(AIS_Shape) ais_shape = new AIS_Shape(shape);
	ais_shape->SetMaterial(Graphic3d_MaterialAspect(Graphic3d_NOM_STEEL));
	//Set Display color 
	ais_shape->SetColor(color);
	// Set Display mode
	ais_shape->SetDisplayMode((int)AIS_DisplayMode::AIS_Shaded);
	// Set Edge Color
	auto drawer = ais_shape->Attributes();
	drawer->SetFaceBoundaryDraw(true);
	Handle(Prs3d_LineAspect) aspect = new Prs3d_LineAspect(Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0);
	drawer->SetFaceBoundaryAspect(aspect);
	//设置高亮模式
	ais_shape->SetHilightMode(1);
	Context->Display(ais_shape, Standard_True);

	


	//interactivemanager->SetSelectionModel(4, ais_shape);
	/*
	* //get default drawer from context
		Handle(Prs3d_Drawer) drawer = myContext->DefaultDrawer();

		//set faces edges visible
		drawer->SetFaceBoundaryDraw(true);
	*/
}

void CAM::DisplayCore::DisplayShape(TopoDS_Shape shape, double transparency, Graphic3d_MaterialAspect material, Standard_Boolean theToUpdateViewer)
{
	Handle(AIS_Shape) ais_shape = new AIS_Shape(shape);
	ais_shape->SetMaterial(Graphic3d_MaterialAspect(Graphic3d_NOM_STEEL));
	// Set Display mode
	ais_shape->SetDisplayMode((int)AIS_DisplayMode::AIS_Shaded);
	// Set Edge Color
	auto drawer = ais_shape->Attributes();
	drawer->SetFaceBoundaryDraw(true);
	Handle(Prs3d_LineAspect) aspect = new Prs3d_LineAspect(Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0);
	drawer->SetFaceBoundaryAspect(aspect);
	//设置高亮模式
	ais_shape->SetHilightMode(1);
	Context->Display(ais_shape, Standard_True);
	//interactivemanager->SetSelectionModel(4, ais_shape);
	/*
	* //get default drawer from context
		Handle(Prs3d_Drawer) drawer = myContext->DefaultDrawer();

		//set faces edges visible
		drawer->SetFaceBoundaryDraw(true);
	*/

}

void CAM::DisplayCore::DisplayShape(TopoDS_Shape shape, Quantity_Color color, Aspect_TypeOfLine theType, Standard_Boolean theToUpdateViewer)
/*
		Aspect_TOL_EMPTY = -1, //!< hidden
		Aspect_TOL_SOLID =  0, //!< continuous
		Aspect_TOL_DASH,       //!< dashed 2.0,1.0 (MM)
		Aspect_TOL_DOT,        //!< dotted 0.2,0.5 (MM)
		Aspect_TOL_DOTDASH,    //!< mixed  10.0,1.0,2.0,1.0 (MM)
		Aspect_TOL_USERDEFINED //!< defined by Users
*/
{
	// 遍历 Wire 中的每个 Edge
	for (TopExp_Explorer expEdge(shape, TopAbs_EDGE); expEdge.More(); expEdge.Next()) {
		TopoDS_Edge edge = TopoDS::Edge(expEdge.Current());

		// 获取 Edge 的几何曲线
		double first, last;
		Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, first, last);
		// 尝试将其转换为 Geom_Line
		Handle(Geom_Line) line = Handle(Geom_Line)::DownCast(curve->Reversed());
		if (!line.IsNull())
		{
			Handle(AIS_Line) ais_shape = new AIS_Line(line);
			auto drawer = ais_shape->Attributes();
			Handle(Prs3d_LineAspect) aspect = new Prs3d_LineAspect(color, theType, 1.0);
			drawer->SetLineAspect(aspect);
			ais_shape->Attributes()->SetLineAspect(aspect);
			ais_shape->SetHilightMode(1);
			Context->Display(ais_shape, Standard_True);
		}
	}

}



void CAM::DisplayCore::DisplayShape(TopoDS_Shape theshape, Aspect_TypeOfLine theType, Quantity_Color color, Standard_Integer LineWidth, string shapeLable, Standard_Boolean theToUpdateViewer)
{
	Handle(AIS_Shape) ais_shape = new AIS_Shape(theshape);
	/*设置显色*/
	ais_shape->SetColor(color);
	/*设置线宽*/
	ais_shape->SetWidth(LineWidth);
	//ais_shape->SetDisplayMode((int)AIS_DisplayMode::AIS_Shaded);
	auto drawer = ais_shape->Attributes();
	//drawer->SetFaceBoundaryDraw(true);
	Handle(Prs3d_LineAspect) aspect = new Prs3d_LineAspect(color, theType, LineWidth);
	drawer->SetLineAspect(aspect);
	ais_shape->Attributes()->SetLineAspect(aspect);
	ais_shape->SetHilightMode(1);
	Context->Display(ais_shape, Standard_True);
	ShapeManeger[shapeLable] = new shape(ais_shape);
}



void CAM::DisplayCore::Read_step_file_with_names_colors1(string filename)
{
	
	list<int> assemble_relation_list;
	map<TopoDS_Shape, string> output_shapes;
	
}

int CAM::DisplayCore::Read_step_file(string filepath,string filename)
{
	Quantity_Color color(0.5, 0.6, 0.7, Quantity_TOC_RGB);


	// 创建 STEP 文件读取器
	STEPControl_Reader reader;

	// 加载 STEP 文件
	IFSelect_ReturnStatus status = reader.ReadFile(filepath.c_str());
	
	

	// 检查文件是否成功加载
	if (status != IFSelect_RetDone) {
		std::cerr << "Error: Unable to read the STEP file." << std::endl;
		
		
		
	}

	// 将 STEP 文件内容转换为 OpenCASCADE 的内部表示
	Standard_Integer nbr = reader.TransferRoots();
	if (nbr == 0) {

		std::cerr << "Error: No shapes found in the STEP file." << std::endl;

	}

	// 获取转换后的形状
	TopoDS_Shape topods_shape = reader.OneShape();
	Handle(AIS_Shape) ais_shape = new AIS_Shape(topods_shape);
	this->ShapeManeger[filename] = new shape(ais_shape);
	this->DisplayShape(topods_shape, color, 1);

	return 0;
}

int CAM::DisplayCore::Displaytriehedron()
{
	Handle(Geom_Axis2Placement) axis = new Geom_Axis2Placement(gp::XOY());
	Handle(AIS_Trihedron) triehedron = new AIS_Trihedron(axis);
	/*设置轴的主体颜色*/
	triehedron->SetXAxisColor(Quantity_Color(Quantity_NOC_RED));
	triehedron->SetYAxisColor(Quantity_Color(Quantity_NOC_GREEN));
	triehedron->SetAxisColor(Quantity_Color(Quantity_NOC_BLUE1));
	triehedron->SetTextColor(Quantity_Color(Quantity_NOC_BLACK));
	/*设置箭头颜色*/
	triehedron->SetArrowColor(Prs3d_DatumParts_XAxis, Quantity_Color(Quantity_NOC_RED));
	triehedron->SetArrowColor(Prs3d_DatumParts_YAxis, Quantity_Color(Quantity_NOC_GREEN));
	triehedron->SetArrowColor(Prs3d_DatumParts_ZAxis, Quantity_Color(Quantity_NOC_BLUE1));

	Context->Display(triehedron, 0, 3, true);
	ShapeManeger["世界坐标系"] = new shape(triehedron);
	//Handle(drawer) = triehedron.Attributes();
	return 0;
}

int CAM::DisplayCore::DisplaytWCSriehedron(const gp_Pnt& P, const gp_Dir& Vx, const gp_Dir& Vy)
{
	gp_Dir Vz = Vx.Crossed(Vy);
	Handle(Geom_Axis2Placement) axis = new Geom_Axis2Placement(P,Vz,Vx);
	Handle(AIS_Trihedron) triehedron = new AIS_Trihedron(axis);
	/*设置轴的主体颜色*/
	triehedron->SetXAxisColor(Quantity_Color(Quantity_NOC_RED));
	triehedron->SetYAxisColor(Quantity_Color(Quantity_NOC_GREEN));
	triehedron->SetAxisColor(Quantity_Color(Quantity_NOC_BLUE1));
	/*设置文字颜色*/
	triehedron->SetTextColor(Quantity_Color(Quantity_NOC_BLACK));
	/*设置箭头颜色*/
	triehedron->SetArrowColor(Prs3d_DatumParts_XAxis,Quantity_Color(Quantity_NOC_RED));
	triehedron->SetArrowColor(Prs3d_DatumParts_YAxis, Quantity_Color(Quantity_NOC_GREEN));
	triehedron->SetArrowColor(Prs3d_DatumParts_ZAxis, Quantity_Color(Quantity_NOC_BLUE1));
	/*设置坐标系轴标签*/
	triehedron->SetLabel(Prs3d_DatumParts_XAxis, "XM");
	triehedron->SetLabel(Prs3d_DatumParts_YAxis, "YM");
	triehedron->SetLabel(Prs3d_DatumParts_ZAxis, "ZM"); 
	Context->Display(triehedron, 0, 3, true);
	ShapeManeger["工作坐标系"] = new shape(triehedron);
	//Handle(drawer) = triehedron.Attributes();
	return 0;
}

int CAM::DisplayCore::DisplayPlane()
{	
	Handle(Geom_Plane)plane = new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0, 1, 0));
	Handle(AIS_Plane)ais_plane_xz = new AIS_Plane(plane, true);
	Handle(Prs3d_PlaneAspect) plan_asp = new Prs3d_PlaneAspect();
	plan_asp->SetPlaneLength(200, 200);
	Handle(Prs3d_LineAspect) line_asp = plan_asp->EdgesAspect();
	line_asp->SetColor(Quantity_Color(Quantity_NOC_GREEN));
	ais_plane_xz->Attributes()->SetPlaneAspect(plan_asp);
	ais_plane_xz->SetTypeOfSensitivity(Select3D_TOS_INTERIOR);
	Context->Display(ais_plane_xz, 0, 3, true);
	ShapeManeger["基准面XZ"] = new shape(ais_plane_xz);

	plane = new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1, 0, 0));
	Handle(AIS_Plane)ais_plane_yz = new AIS_Plane(plane, true);
	ais_plane_yz->Attributes()->SetPlaneAspect(plan_asp);
	ais_plane_yz->SetTypeOfSensitivity(Select3D_TOS_INTERIOR);
	Context->Display(ais_plane_yz, 0, 3, true);
	ShapeManeger["基准面YZ"] = new shape(ais_plane_yz);

	plane = new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0, 0, 1));
	Handle(AIS_Plane)ais_plane_xy = new AIS_Plane(plane);
	ais_plane_xy->Attributes()->SetPlaneAspect(plan_asp);
	ais_plane_xy->SetTypeOfSensitivity(Select3D_TOS_INTERIOR);
	Context->Display(ais_plane_xy, 0, 3, true);
	ShapeManeger["基准面XY"] = new shape(ais_plane_xy);
	return 0;
}

int DisplayCore::DisplayPlane(Geom_Plane theplane)
{
	Handle(Geom_Plane)plane = new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0, 1, 0));
	Handle(AIS_Plane)ais_plane_xz = new AIS_Plane(plane, true);
	Handle(Prs3d_PlaneAspect) plan_asp = new Prs3d_PlaneAspect();
	plan_asp->SetPlaneLength(200, 200);
	Handle(Prs3d_LineAspect) line_asp = plan_asp->EdgesAspect();
	line_asp->SetColor(Quantity_Color(Quantity_NOC_GREEN));
	ais_plane_xz->Attributes()->SetPlaneAspect(plan_asp);
	ais_plane_xz->SetTypeOfSensitivity(Select3D_TOS_INTERIOR);
	Context->Display(ais_plane_xz, 0, 3, true);
	//ShapeManeger["基准面XZ"] = new shape(ais_plane_xz);
	return 0;
}

int CAM::DisplayCore::DisplayCube()
{	
	Handle(AIS_ViewCube) aisviewcube= new AIS_ViewCube();
	ShapeManeger["cube"] = new shape(aisviewcube);
	Handle(Graphic3d_TransformPers) transformPersistence =
		new Graphic3d_TransformPers(Graphic3d_TMF_ZoomPers, gp_Pnt(1000.0, 1000.0, 1000.0));
	//ShapeManeger["cube"]->AISViewCube->SetTransformPersistence(transformPersistence);
	Context->Display(ShapeManeger["cube"]->AISViewCube, true);
	return 0;
}

int CAM::DisplayCore::DisplayAxis(Vector_Type thevector,gp_Pnt point, gp_Dir diretion, Quantity_Color color, Standard_Real length, Standard_Real width)
{
	gp_Ax1 zAxis(point, diretion); // Axis at origin pointing in Z direction
	Handle(AIS_Axis) axis = new AIS_Axis(zAxis, length);
	auto drawer = axis->Attributes();
	Handle(Prs3d_LineAspect) aspect = new Prs3d_LineAspect(color, Aspect_TOL_SOLID, width);
	drawer->SetLineAspect(aspect);
	axis->SetWidth(2);
	axis->Attributes()->SetLineAspect(aspect);
	axis->SetDisplayAspect(aspect);
	Context->Display(axis, Standard_True);
	switch (thevector)
	{
	case CAM::X_Vector:
		ShapeManeger["X方向"] = new shape(axis);
		break;
	case CAM::Y_Vector:
		ShapeManeger["Y方向"] = new shape(axis);
		break;
	case CAM::Z_Vector:
		ShapeManeger["Z方向"] = new shape(axis);
		break;
	}
	return 0;
}





void CAM::DisplayCore::HidePlane()
{
	Context->Erase(ShapeManeger["基准面XZ"]->AisPlane, true);
	Context->Erase(ShapeManeger["基准面YZ"]->AisPlane, true);
	Context->Erase(ShapeManeger["基准面XY"]->AisPlane, true);

}

void CAM::DisplayCore::RemoveAxis(Vector_Type thevector)
{
	switch (thevector)
	{
	case CAM::X_Vector:
		if (!ShapeManeger["X方向"]->AISAxis.IsNull())
		{
			Context->Remove(ShapeManeger["X方向"]->AISAxis, Standard_True);
		}
		break;
	case CAM::Y_Vector:
		if (!ShapeManeger["Y方向"]->AISAxis.IsNull())
		{
			Context->Remove(ShapeManeger["Y方向"]->AISAxis, Standard_True);
		}
		break;
	case CAM::Z_Vector:
		if (!ShapeManeger["Z方向"]->AISAxis.IsNull())
		{
			Context->Remove(ShapeManeger["Z方向"]->AISAxis, Standard_True);
		}
		break;
	}

}






CAM::shape::shape(Handle(AIS_Shape) aisShape)
{
	AisShape = aisShape;
}

CAM::shape::shape(Handle(AIS_Trihedron) aisrihedron)
{
	Aisrihedron = aisrihedron;
}

CAM::shape::shape(Handle(AIS_Plane) aisplane)
{
	AisPlane = aisplane;
}

CAM::shape::shape(Handle(AIS_ViewCube) aisviewcube)
{

	AISViewCube = aisviewcube;
}

CAM::shape::shape(Handle(AIS_Axis) aisvaxis)
{
	AISAxis = aisvaxis;
}


void* CAM::shape::Value()
{
	return NULL;
}

