/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWSnapshotHandler.h 560 2007-12-17 13:20:51Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-12-17 14:20:51 +0100 (Mon, 17 Dec 2007) $
Version:   $Revision: 560 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkKWSnapshotHandler_h
#define _vtkKWSnapshotHandler_h

#include "vtkINRIA3DConfigure.h"

#include "vtkKWDialog.h"
#include <vtkCollection.h>

#include <vector>
#include <string>

class vtkKWRenderWidget;
class vtkKWRadioButton;
class vtkKWCameraAnimationWidget;

class vtkViewImage2D;
class vtkViewImage3D;
class vtkViewImage;
class vtkKWTimeAnimationWidget;
class vtkKWFrame;


class KW_ADDON_EXPORT vtkKWSnapshotHandler : public vtkKWDialog
{
public:
  static vtkKWSnapshotHandler* New();
  vtkTypeRevisionMacro(vtkKWSnapshotHandler,vtkKWDialog);
  
  // Description:


  //BTX
  enum SnapshotTypeId
  {
    SNAPSHOTTYPE_SLICE = 0,
    SNAPSHOTTYPE_CAMERA,
    SNAPSHOTTYPE_TIME
  };
  //ETX
  
  vtkGetMacro (SnapshotType, unsigned int);
  void SetSnapshotType (unsigned int id);
  
  vtkGetObjectMacro (RenderWidget, vtkKWRenderWidget);
  virtual void SetRenderWidget (vtkKWRenderWidget* widget);
  vtkGetObjectMacro (View, vtkViewImage);
  virtual void SetView (vtkViewImage* view);
  
  virtual void Update (void);

  static void Snap (vtkKWRenderWidget* widget);

  void SetTimeUpdateCommand( vtkObject *object, const char *method);
  void SetTimeSettings (double timemin, double timemax, unsigned int maxnumber);

  vtkGetMacro (AllowSliceType, int);
  vtkSetClampMacro (AllowSliceType, int, 0, 1);
  vtkBooleanMacro (AllowSliceType, int);

  vtkGetMacro (AllowCameraType, int);
  vtkSetClampMacro (AllowCameraType, int, 0, 1);
  vtkBooleanMacro (AllowCameraType, int);

  vtkGetMacro (AllowTimeType, int);
  vtkSetClampMacro (AllowTimeType, int, 0, 1);
  vtkBooleanMacro (AllowTimeType, int);
  
protected:
  vtkKWSnapshotHandler();
  ~vtkKWSnapshotHandler();

  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void CreateButtons();
  
  vtkKWFrame* ButtonFrame;
  vtkKWFrame* AnimationFrame;

  vtkKWRadioButton* RadioButtonSlice;
  vtkKWRadioButton* RadioButtonCamera;
  vtkKWRadioButton* RadioButtonTime;
  
  


private:
  vtkKWSnapshotHandler(const vtkKWSnapshotHandler&);   // Not implemented.
  void operator=(const vtkKWSnapshotHandler&);        // Not implemented.

  unsigned int SnapshotType;
  vtkKWCameraAnimationWidget* CameraAnimationWidget;
  vtkKWTimeAnimationWidget* TimeAnimationWidget;

  vtkKWRenderWidget* RenderWidget;
  vtkViewImage* View;

  int AllowSliceType;
  int AllowCameraType;
  int AllowTimeType;
  
  
  
};

#endif
