/*
*    Copyright (c) 2018 Shing Liu All Rights Reserved.
*
*           File : OccView.cpp
*         Author : Shing Liu(eryar@163.com)
*           Date : 2018-01-08 21:00
*        Version : OpenCASCADE7.2.0 & Qt5.7.1
*
*    Description : Qt widget for OpenCASCADE viewer.
*/

#include <opencascade/OpenGl_GraphicDriver.hxx>
#include "occView.h"
#include <QMenu>
#include <QMouseEvent>
#include <QRubberBand>
#include <QStyleFactory>
#include <QtCore/qnamespace.h>
#include <opencascade/V3d_View.hxx>
#include <opencascade/Aspect_Handle.hxx>
#include <opencascade/Aspect_DisplayConnection.hxx>
#include <QDebug>
#include <opencascade/Standard_Handle.hxx>
#include <opencascade/WNT_Window.hxx>


#ifdef WNT
    #include <opencascade/WNT_Window.hxx>
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
    #include <opencascade/Cocoa_Window.hxx>
#else
    #undef Bool
    #undef CursorShape
    #undef None
    #undef KeyPress
    #undef KeyRelease
    #undef FocusIn
    #undef FocusOut
    #undef FontChange
    #undef Expose
    #include <opencascade/Xw_Window.hxx>
#endif

static Handle(Graphic3d_GraphicDriver)& GetGraphicDriver()
{
  static Handle(Graphic3d_GraphicDriver) aGraphicDriver;
  return aGraphicDriver;
}


CAM::OccView::OccView(QWidget* parent )
    : QWidget(parent),
    myXmin(0),
    myYmin(0),
    myXmax(0),
    myYmax(0),
    myCurrentMode(CurAction3d_DynamicRotation),
    myDegenerateModeIsOn(Standard_True),
    myRectBand(NULL)
{
    // No Background
    setBackgroundRole( QPalette::NoRole );

    // set focus policy to threat QContextMenuEvent from keyboard  
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    // Enable the mouse tracking, by default the mouse tracking is disabled.
    setMouseTracking( true );
    //set course  icons
    course["pan"] = QPixmap(":/images/images/cursor-pan.png");
    course["pan"].scaled(QSize(3,3),Qt::KeepAspectRatio);

    course["rotate"] = QPixmap(":/images/images/cursor-rotate.png");
    course["rotate"].scaled(QSize(3, 3), Qt::KeepAspectRatio);

    course["scale"] = QPixmap(":/images/images/ccursor-magnify.png");
    course["scale"].scaled(QSize(3, 3), Qt::KeepAspectRatio);

    //setCursor(QCursor(course["pan"]));
   // connect(this, SIGNAL(mySignal(int)), this, SLOT(mySlot()));
    init();
}



void CAM::OccView::init()
{
    // Create Aspect_DisplayConnection
    Handle(Aspect_DisplayConnection) aDisplayConnection =
            new Aspect_DisplayConnection();

    // Get graphic driver if it exists, otherwise initialise it
    if (GetGraphicDriver().IsNull())
    {
        GetGraphicDriver() = new OpenGl_GraphicDriver(aDisplayConnection);
    }

    // Get window handle. This returns something suitable for all platforms.
    WId window_handle = (WId) winId();

    // Create appropriate window for platform
    //#ifdef WNT
    Handle(WNT_Window) wind = new WNT_Window((Aspect_Handle) window_handle);
    //#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
        //Handle(Cocoa_Window) wind = new Cocoa_Window((NSView *) window_handle);
    //#else
      // Handle(Xw_Window) wind = new Xw_Window(aDisplayConnection, (Window) window_handle);
    //#endif



    // Create V3dViewer and V3d_View

    /*3D显示窗口*/
    myViewer = new V3d_Viewer(GetGraphicDriver());
    myViewer->SetDefaultLights();
    myViewer->SetLightOn();
    myViewer->SetDefaultViewProj(V3d_XposYposZpos);
    myViewer->SetDefaultLights();
    myViewer->SetLightOn();
    myView = myViewer->CreateView();
	/*获取渲染参数*/
	Graphic3d_RenderingParams& aRenderParams = myView->ChangeRenderingParams();
	/*设置抗锯齿模式*/
	aRenderParams.ToSmoothInterlacing = Standard_True;
	aRenderParams.IsAntialiasingEnabled = Standard_True;

    myView->SetWindow(wind);
    if (!wind->IsMapped()) wind->Map();
    //myView->SetProj(V3d_Xpos);//设置视图
    // Create AISInteractiveContext
    myContext = new AIS_InteractiveContext(myViewer);
	
    // Set up lights etc
    myViewer->SetDefaultLights();
    myViewer->SetLightOn();
	double R1,G1,B1,R2,G2,B2;

    R1 = 135 / 255.0, G1 = 150 / 255.0, B1 = 170 / 255.0;
    R2 = 195 / 255.0, G2 = 200 / 255.0, B2 = 210 / 255.0;

	Quantity_Color background_1(R1,G1,B1, Quantity_TOC_RGB);
	Quantity_Color background_2(R2,G2,B2, Quantity_TOC_RGB);

    //myView->SetBackgroundColor(Quantity_NOC_BLACK);
	//背景渐变色设置

    Standard_CString FileName = "..\\images\\Cloudy.png";
	myView->SetBgGradientColors(background_1, background_2, Aspect_GFM_VER, Standard_True);
    myView->SetBackgroundImage(FileName,Aspect_FM_STRETCH,  true);
    myView->MustBeResized();
    myView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_BLACK, 0.2, V3d_ZBUFFER);
    myView->Redraw();
    myContext->SetDisplayMode(1, Standard_True);
}

const Handle(AIS_InteractiveContext)& CAM::OccView::getContext() const
{
    return myContext;
}

const Handle(V3d_Viewer)& CAM::OccView::getV3dviewer() const
{
	return myViewer;
}

const Handle(V3d_View)& CAM::OccView::getV3dview() const
{
	return myView;
}


/*!
Get paint engine for the OpenGL viewer. [ virtual public ]
*/
QPaintEngine* CAM::OccView::paintEngine() const
{
    return 0;
}

void CAM::OccView::paintEvent( QPaintEvent* /*theEvent*/ )
{
    myView->Redraw();
}

void CAM::OccView::resizeEvent( QResizeEvent* /*theEvent*/ )
{
    if( !myView.IsNull() )
    {
        myView->MustBeResized();
    }
}

void CAM::OccView::fitAll( void )
{
    myView->FitAll();
    myView->ZFitAll();
    myView->Redraw();
}

void CAM::OccView::reset( void )
{
    myView->Reset();
}

void CAM::OccView::pan( void )
{
    myCurrentMode = CurAction3d_DynamicPanning;
}

void CAM::OccView::zoom( void )
{
    myCurrentMode = CurAction3d_DynamicZooming;
}

void CAM::OccView::rotate( void )
{
    myCurrentMode = CurAction3d_DynamicRotation;
}

void CAM::OccView::mousePressEvent( QMouseEvent* theEvent )
{		
    /*鼠标左键单击*/
    if (theEvent->button() == Qt::LeftButton && theEvent->modifiers()!= Qt::ShiftModifier)
    {
		emit mousePressSignal(42);
        onLButtonDown((theEvent->buttons() | theEvent->modifiers()), theEvent->pos());
    }
	/*shift 反选事件*/
	else if (theEvent->button() == Qt::LeftButton && theEvent->modifiers() == Qt::ShiftModifier)
	{
		emit mousePressAndShiftSignal(14);
		onRButtonDown((theEvent->buttons() | theEvent->modifiers()), theEvent->pos());
	}

    else if (theEvent->button() == Qt::MiddleButton && !(theEvent->buttons() & Qt::RightButton))
    {
            this->setCursor(QCursor(course["rotate"]));
            onMButtonDown((theEvent->buttons() | theEvent->modifiers()), theEvent->pos());
    }
    else if (theEvent->button() == Qt::RightButton && !(theEvent->buttons() & Qt::MiddleButton))
    {
            onRButtonDown((theEvent->buttons() | theEvent->modifiers()), theEvent->pos());
    }
    

}	

void CAM::OccView::mouseReleaseEvent( QMouseEvent* theEvent )
{
    
    if (theEvent->button() == Qt::LeftButton)
    {
        emit mouseReleseSignal(42);
        onLButtonUp(theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
    }
    else if (theEvent->button() == Qt::MiddleButton)
    {
        onMButtonUp(theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
    }
    else if (theEvent->button() == Qt::RightButton)
    {
        onRButtonUp(theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
    }

    this->setCursor(Qt::ArrowCursor);
}

void CAM::OccView::mouseMoveEvent( QMouseEvent * theEvent )
{
    
    onMouseMove(theEvent, theEvent->pos());
}

void CAM::OccView::wheelEvent( QWheelEvent * theEvent )
{
    onMouseWheel(theEvent->buttons(), theEvent->angleDelta().y(), theEvent->position().toPoint());
}

void CAM::OccView::onLButtonDown( const int /*theFlags*/, const QPoint thePoint )
{
    // Save the current mouse coordinate in min.
    myXmin = thePoint.x();
    myYmin = thePoint.y();
    myXmax = thePoint.x();
    myYmax = thePoint.y();
	
}

void CAM::OccView::onMButtonDown( const int /*theFlags*/, const QPoint thePoint )
{
    // Save the current mouse coordinate in min.
    myXmin = thePoint.x();
    myYmin = thePoint.y();
    myXmax = thePoint.x();
    myYmax = thePoint.y();
    mid_button_press_x= thePoint.x();
    mid_button_press_y= thePoint.y();


    if (myCurrentMode == CurAction3d_DynamicRotation)
    {
        myView->StartRotation(thePoint.x(), thePoint.y());
    }
}

void CAM::OccView::onRButtonDown( const int /*theFlags*/, const QPoint thePoint )
{
	// Save the current mouse coordinate in min.
	myXmin = thePoint.x();
	myYmin = thePoint.y();
	myXmax = thePoint.x();
	myYmax = thePoint.y();
}

void CAM::OccView::onMouseWheel( const int /*theFlags*/, const int theDelta, const QPoint thePoint )
{
    double aFactor = 10;
    Standard_Integer aX = thePoint.x();
    Standard_Integer aY = thePoint.y();
    

    if (theDelta < 0)//通过重写设置显示比例进行缩放
    {
        zoom_factor=1+aFactor/100;
    }
    else
    {
        zoom_factor = 1 - aFactor / 100;
       
    }
    myView->SetZoom(zoom_factor);
    //myView->Zoom(thePoint.x(), thePoint.y(), aX, aY);
}

void CAM::OccView::addItemInPopup( QMenu* /*theMenu*/ )
{
}

void CAM::OccView::popup( const int /*x*/, const int /*y*/ )
{
}

void CAM::OccView::onLButtonUp( const int theFlags, const QPoint thePoint )
{
    // Hide the QRubberBand
    if (myRectBand)
    {
        myRectBand->hide();
    }

    // Ctrl for multi selection.
    if (thePoint.x() == myXmin && thePoint.y() == myYmin)
    {
        if (theFlags & Qt::ControlModifier)
        {
            multiInputEvent(thePoint.x(), thePoint.y());
        }
        else
        {
            inputEvent(thePoint.x(), thePoint.y());
        }
    }

}

void CAM::OccView::onMButtonUp( const int /*theFlags*/, const QPoint thePoint )
{
    if (thePoint.x() == myXmin && thePoint.y() == myYmin)

    {
        if (thePoint.x()!=mid_button_press_x || thePoint.y()!=mid_button_press_y )
            panByMiddleButton(thePoint);
       
    }
}

void CAM::OccView::onRButtonUp( const int /*theFlags*/, const QPoint thePoint )
{
    popup(thePoint.x(), thePoint.y());
}

void CAM::OccView::onMouseMove(QMouseEvent* event, const QPoint thePoint )
{   
	emit mouseMoveSignal(42);
	_dragPosX = event->pos().x();
	_dragPosY = event->pos().y();
	//myView->ProjReferenceAxe(_dragPosX, _dragPosY, XP, YP, ZP, XV, YV, ZV);
	
    
    // Draw the rubber band.
    if (event->buttons()==Qt::LeftButton)
    {
        drawRubberBand(myXmin, myYmin, thePoint.x(), thePoint.y());

        dragEvent(thePoint.x(), thePoint.y());
    }
	/*Pan*/ 
	if (event->buttons() ==( Qt::RightButton|Qt::MiddleButton))
	{       
         PanStatus = 1;
         this->setCursor(QCursor(course["pan"]));
		 myView->Pan(thePoint.x() - myXmax, myYmax - thePoint.y());
         myXmax = thePoint.x();
         myYmax = thePoint.y();
        

	}

    // Ctrl for multi selection.
    if (event->buttons() & Qt::ControlModifier)
    {
        multiMoveEvent(thePoint.x(), thePoint.y());
    }
    else
    {
        moveEvent(thePoint.x(), thePoint.y());
    }

    // Middle button.
    if (event->buttons()==Qt::MiddleButton && (event->buttons() != Qt::RightButton))
    {

        switch (myCurrentMode)
        {

        case CurAction3d_DynamicRotation:
            if (PanStatus!=1)
            {
				myView->Rotation(thePoint.x(), thePoint.y());
				break;
            }
            

        case CurAction3d_DynamicZooming:
            myView->Zoom(myXmin, myYmin, thePoint.x(), thePoint.y());
            break;

        case CurAction3d_DynamicPanning:
            myView->Pan(thePoint.x() - myXmax, myYmax - thePoint.y());
            myXmax = thePoint.x();
            myYmax = thePoint.y();
            break;

         default:
            break;
        }
    }
    PanStatus = 0;

}

void CAM::OccView::dragEvent( const int x, const int y )
{
    //myContext->Select(myXmin, myYmin, x, y, myView, Standard_True);

    emit selectionChanged();
}

void CAM::OccView::multiDragEvent( const int x, const int y )
{
    //myContext->ShiftSelect(myXmin, myYmin, x, y, myView, Standard_True);

    emit selectionChanged();

}

void CAM::OccView::inputEvent( const int x, const int y )
{
    Q_UNUSED(x);
    Q_UNUSED(y);

    //myContext->Select(Standard_True);

    emit selectionChanged();
}

void CAM::OccView::multiInputEvent( const int x, const int y )
{
    Q_UNUSED(x);
    Q_UNUSED(y);

   // myContext->ShiftSelect(Standard_True);

    emit selectionChanged();
}

void CAM::OccView::moveEvent( const int x, const int y )
{
    myContext->MoveTo(x, y, myView, Standard_True);
}

void CAM::OccView::multiMoveEvent( const int x, const int y )
{
    myContext->MoveTo(x, y, myView, Standard_True);
}

void CAM::OccView::drawRubberBand( const int minX, const int minY, const int maxX, const int maxY )
{
    QRect aRect;

    // Set the rectangle correctly.
    (minX < maxX) ? (aRect.setX(minX)) : (aRect.setX(maxX));
    (minY < maxY) ? (aRect.setY(minY)) : (aRect.setY(maxY));

    aRect.setWidth(abs(maxX - minX));
    aRect.setHeight(abs(maxY - minY));

    if (!myRectBand)
    {
        myRectBand = new QRubberBand(QRubberBand::Rectangle, this);

        // setStyle is important, set to windows style will just draw
        // rectangle frame, otherwise will draw a solid rectangle.
        myRectBand->setStyle(QStyleFactory::create("windows"));
    }

    myRectBand->setGeometry(aRect);
    myRectBand->show();
}

void CAM::OccView::panByMiddleButton( const QPoint& thePoint )
{
    Standard_Integer aCenterX = 0;
    Standard_Integer aCenterY = 0;

    QSize aSize = size();

    aCenterX = aSize.width() / 2;
    aCenterY = aSize.height() / 2;

    myView->Pan(aCenterX - thePoint.x(), thePoint.y() - aCenterY);
}

void CAM::OccView::mySlot()
{
    qDebug() << "The value is:";
}

int CAM::OccView::View_Top()
{
    myView->SetProj(V3d_Zpos);
    return 0;
}

int CAM::OccView::View_Bottom()
{
    myView->SetProj(V3d_Zneg);
    return 0;
}

int CAM::OccView::View_Left()
{
    myView->SetProj(V3d_Xneg);
    return 0;
}

int CAM::OccView::View_Right()
{
    myView->SetProj(V3d_Xpos);
    return 0;
}

int CAM::OccView::View_Front()
{
    myView->SetProj(V3d_Yneg);
    return 0;
}

int CAM::OccView::View_Rear()
{
    myView->SetProj(V3d_Ypos);
    return 0;
}

int CAM::OccView::View_Iso()
{
    myView->SetProj(V3d_XposYnegZpos);
    return 0;
}
