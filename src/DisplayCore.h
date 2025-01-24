#pragma once
#include "occView.h"
#include <opencascade/AIS_Plane.hxx>
#include <opencascade/AIS_Shape.hxx>
#include <opencascade/AIS_Trihedron.hxx>
#include <opencascade/AIS_ViewCube.hxx>
#include <opencascade/AIS_Axis.hxx>
#include <opencascade/TopoDS_Shape.hxx>
using namespace std;


class CAM::OccView;

namespace CAM
{
	enum Vector_Type
	{
		X_Vector,
		Y_Vector,
		Z_Vector

	};

	class shape
	{

	public:
		shape(Handle(AIS_Shape) aisShape);
		shape(Handle(AIS_Trihedron) aisrihedron);
		shape(Handle(AIS_Plane) aisplane);
		shape(Handle(AIS_ViewCube) aisviewcube); 
		shape(Handle(AIS_Axis) aisvaxis);

		~shape() = default;
		void* Value();

	public:
		Handle(AIS_Shape) AisShape {nullptr};
		Handle(AIS_Trihedron) Aisrihedron {nullptr};
		Handle(AIS_Plane) AisPlane {nullptr};
		Handle(AIS_ViewCube) AISViewCube {nullptr};
		Handle(AIS_Axis) AISAxis {nullptr};


		shape() = default;
	};

	class DisplayCore
	{
	public:
		Handle(AIS_InteractiveContext) Context;
	public:
		DisplayCore(OccView* parent);
		~DisplayCore()=default;
		void DisplayShape(TopoDS_Shape shape, Quantity_Color color, double transparency, Graphic3d_MaterialAspect material = Graphic3d_NOM_STEEL, Standard_Boolean theToUpdateViewer = true);
		void DisplayShape(TopoDS_Shape shape, double transparency, Graphic3d_MaterialAspect material = Graphic3d_NOM_STEEL, Standard_Boolean theToUpdateViewer = true);
		void DisplayShape(TopoDS_Shape shape, Quantity_Color color, Aspect_TypeOfLine theType, Standard_Boolean theToUpdateViewer = true);
		void DisplayShape(TopoDS_Shape shape, Aspect_TypeOfLine theType, Quantity_Color color, Standard_Integer LineWidth, string shapeLable="object",Standard_Boolean theToUpdateViewer = true);
		void Read_step_file_with_names_colors1(string filename);
		int Read_step_file(string filepath, string filename);
		int Displaytriehedron();
		int DisplaytWCSriehedron(const gp_Pnt& P, const gp_Dir& Vx, const gp_Dir& Vy);
		int DisplayPlane();
		int DisplayPlane(Geom_Plane theplane);
		int DisplayCube();
		int DisplayAxis(Vector_Type thevector,gp_Pnt point,gp_Dir diretion, Quantity_Color color,Standard_Real length=50,Standard_Real width=3.5);
		void HidePlane();
		void RemoveAxis(Vector_Type thevector);

	public:
		map<string, shape*> ShapeManeger;
		OccView* myOccView;
		TopoDS_Shape  currentShape;

		

	};

}