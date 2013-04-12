/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkViewImage2D.h 1218 2009-08-04 11:29:59Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-08-04 13:29:59 +0200 (mar, 04 aoû 2009) $
Version:   $Revision: 1218 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#pragma once

#include "vtkImageViewExport.h"
#include "vtkObject.h"
#include "vtkCommand.h"

class vtkImageView2D;
class vtkDataSet2DWidgetCommand;
class vtkActor;
class vtkPolyDataMapper;
class vtkPointSet;
class vtkRenderer;
class vtkCutter;
class vtkPlane;


class VTK_IMAGEVIEW_EXPORT vtkDataSet2DWidget : public vtkObject
{
public:
  // Description:
  // Instantiate this class.
  static vtkDataSet2DWidget *New();

  // Description:
  // Standard VTK class macros.
  vtkTypeRevisionMacro(vtkDataSet2DWidget,vtkObject);
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

  void SetSource (vtkPointSet* arg);
  vtkGetObjectMacro (Source, vtkPointSet);
  
  vtkGetObjectMacro (Actor, vtkActor);
  vtkGetObjectMacro (ImplicitPlane, vtkPlane);
  
protected:
  vtkDataSet2DWidget();
  ~vtkDataSet2DWidget();

  vtkImageView2D* ImageView;
  vtkDataSet2DWidgetCommand* Command;

  vtkPointSet*       Source;
  vtkPolyDataMapper* Mapper;
  vtkActor*          Actor;
  vtkRenderer*       Renderer;
  vtkCutter*         Cutter;
  vtkPlane*          ImplicitPlane;
  int                Enabled;

private:
  vtkDataSet2DWidget(const vtkDataSet2DWidget&);  //Not implemented
  void operator=(const vtkDataSet2DWidget&);  //Not implemented
};




//BTX
class VTK_IMAGEVIEW_EXPORT vtkDataSet2DWidgetCommand : public vtkCommand
{

 public:

  static vtkDataSet2DWidgetCommand *New()
    {return new vtkDataSet2DWidgetCommand;};

  // Description:
  // Satisfy the superclass API for callbacks. Recall that the caller is
  // the instance invoking the event; eid is the event id (see
  // vtkCommand.h); and calldata is information sent when the callback
  // was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
  virtual void Execute(vtkObject *caller,unsigned long event, void *vtkNotUsed(callData));

  void SetWidget (vtkDataSet2DWidget* w)
  { this->Widget = w; }
  vtkDataSet2DWidget* GetWidget()
  { return this->Widget; }
  
 protected:

  vtkDataSet2DWidgetCommand()
  {
    this->Widget = 0;
  }
  
  ~vtkDataSet2DWidgetCommand(){};

 private:

  vtkDataSet2DWidget* Widget;
};
//ETX




