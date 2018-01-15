#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <medVtkInriaExport.h>
#include <vtkObject.h>
#include <vtkCommand.h>

class vtkImageView2D;
class vtkAxes2DWidgetCommand;
class vtkActor;
class vtkUnsignedCharArray;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkPlane;
class vtkPoints;
class vtkRenderer;
class vtkRenderWindow;


class MEDVTKINRIA_EXPORT vtkAxes2DWidget : public vtkObject
{
public:
  // Description:
  // Instantiate this class.
  static vtkAxes2DWidget *New();

  // Description:
  // Standard VTK class macros.
  vtkTypeMacro(vtkAxes2DWidget,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Methods for activating this widget. Note that the widget representation
  // must be specified or the widget will not appear.
  // ProcessEvents (On by default) must be On for Enabled widget to respond 
  // to interaction. If ProcessEvents is Off, enabling/disabling a widget 
  // merely affects the visibility of the representation.
  virtual void SetEnabled(int);
  int GetEnabled() {return this->Enabled;}
  void EnabledOn() {this->SetEnabled(1);}
  void EnabledOff() {this->SetEnabled(0);}
  void On() {this->SetEnabled(1);}
  void Off() {this->SetEnabled(0);}
  
  void SetImageView (vtkImageView2D* arg);
  vtkGetObjectMacro (ImageView, vtkImageView2D);

  void SetRenderWindow (vtkRenderWindow *arg);
  vtkGetObjectMacro (RenderWindow, vtkRenderWindow);

  vtkGetObjectMacro (ColorArray, vtkUnsignedCharArray);
  void ComputeLyingPoints (double* pos);
  void ComputePlanes();
  
protected:
  vtkAxes2DWidget();
  ~vtkAxes2DWidget();
    
  vtkImageView2D* ImageView;
  vtkRenderWindow *RenderWindow;
  vtkAxes2DWidgetCommand* Command;
  vtkPlane* PlaneXmin;
  vtkPlane* PlaneXmax;
  vtkPlane* PlaneYmin;
  vtkPlane* PlaneYmax;

  vtkPolyData* Source;
  vtkPoints* Points;
  vtkPolyDataMapper* Mapper;
  vtkActor* Actor;
  vtkRenderer* Renderer;
  
  vtkUnsignedCharArray* ColorArray;
  int Enabled;

private:
  vtkAxes2DWidget(const vtkAxes2DWidget&);  //Not implemented
  void operator=(const vtkAxes2DWidget&);  //Not implemented
};




//BTX
class MEDVTKINRIA_EXPORT vtkAxes2DWidgetCommand : public vtkCommand
{

 public:

  static vtkAxes2DWidgetCommand *New()
    {return new vtkAxes2DWidgetCommand;};

  // Description:
  // Satisfy the superclass API for callbacks. Recall that the caller is
  // the instance invoking the event; eid is the event id (see
  // vtkCommand.h); and calldata is information sent when the callback
  // was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
  virtual void Execute(vtkObject *caller,unsigned long event, void *vtkNotUsed(callData));

  void SetWidget (vtkAxes2DWidget* w)
  { this->Widget = w; }
  vtkAxes2DWidget* GetWidget()
  { return this->Widget; }
  
 protected:

  vtkAxes2DWidgetCommand()
  {
    this->Widget = 0;
  }
  
  ~vtkAxes2DWidgetCommand(){};

 private:

  vtkAxes2DWidget* Widget;
};
//ETX




