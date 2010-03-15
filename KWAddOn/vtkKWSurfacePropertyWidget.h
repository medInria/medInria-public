/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWSurfacePropertyWidget.h 612 2008-01-14 10:59:37Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-01-14 11:59:37 +0100 (Mon, 14 Jan 2008) $
Version:   $Revision: 612 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkKWSurfacePropertyWidget_h
#define __vtkKWSurfacePropertyWidget_h

#include "vtkINRIA3DConfigure.h"

#include <vtkKWFrameWithLabel.h>

class vtkProperty;

class vtkKWHSVColorSelector;
class vtkKWScale;
class vtkKWScaleSet;

class vtkKWComboBox;
class vtkKWComboBoxSet;


class vtkKWSurfaceMaterialPropertyWidget;


class KW_ADDON_EXPORT vtkKWSurfacePropertyWidget : public vtkKWFrame
{
public:
  static vtkKWSurfacePropertyWidget* New();
  vtkTypeRevisionMacro(vtkKWSurfacePropertyWidget,vtkKWFrame);

  virtual void Update();

  virtual void SetPropertyChangedCommand(
    vtkObject *object,const char *method);
  virtual void SetPropertyChangingCommand(
    vtkObject *object,const char *method);

  
  // Description:
  // Callbacks
  virtual void OpacityChangingCallback(double op);
  virtual void ColorChangedCallback(double r, double g, double b);
  virtual void ColorChangingCallback(double r, double g, double b);
  virtual void SurfaceRepresentationChangedCallBack (const char* value);
  virtual void LineWidthChangingCallback (double width);
  virtual void InterpolationModeChangedCallBack(const char* value);

  virtual void PropertyChangingCallback();
  virtual void PropertyChangedCallback();

  virtual void SetProperty (vtkProperty* property);


protected:
  vtkKWSurfacePropertyWidget();
  ~vtkKWSurfacePropertyWidget();

  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void Pack();

  
  virtual void InvokeObjectMethodCommand(const char *command);
  virtual void InvokePropertyChangedCommand();
  virtual void InvokePropertyChangingCommand();
  char* PropertyChangedCommand;
  char* PropertyChangingCommand;
  
  
  vtkKWFrame*            MainFrame;
  vtkKWHSVColorSelector*              HSVColorSelector;
  vtkKWScale*                         OpacityScale;
  vtkKWScaleSet*                         ScaleSet;
  vtkKWSurfaceMaterialPropertyWidget* S_MaterialPropertyWidget;
  vtkKWComboBox*                      SurfaceRepresentationComboBox;
  vtkKWScale*                         LineWidthScale;
  vtkKWComboBox*                      InterpolationModeComboBox;
  vtkKWComboBoxSet*                      ComboBoxSet;
  vtkProperty*                        S_Property;

  
private:
  
  vtkKWSurfacePropertyWidget(const vtkKWSurfacePropertyWidget&);   // Not implemented.
  void operator=(const vtkKWSurfacePropertyWidget&);                 // Not implemented.
  
};

#endif




