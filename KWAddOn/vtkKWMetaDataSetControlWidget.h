/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWMetaDataSetControlWidget.h 612 2008-01-14 10:59:37Z ntoussaint $
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
#ifndef __vtkKWMetaDataSetControlWidget_h
#define __vtkKWMetaDataSetControlWidget_h

#include "vtkINRIA3DConfigure.h"

#include <vtkKWFrameWithLabel.h>

class vtkMetaDataSet;
class vtkDataArray;

class vtkProperty;
class vtkVolumeProperty;

class vtkKWFrameWithScrollbar;
class vtkKWHSVColorSelector;
class vtkKWScale;
class vtkKWSurfaceMaterialPropertyWidget;
class vtkKWVolumeMaterialPropertyWidget;
class vtkKWPushButton;
class vtkKWRadioButtonSetWithLabel;
class vtkKWMenuButtonWithLabel;
class vtkKWColorPresetSelector;
class vtkKWColorTransferFunctionEditor;
class vtkColorTransferFunction;
class vtkKWPushButton;
class vtkKWVolumePropertyPresetSelector;





/* class vtkKWEntryWithLabel; */
/* class vtkKWPushButton; */
/* class vtkKWIcon; */
/*  */


class KW_ADDON_EXPORT vtkKWMetaDataSetControlWidget : public vtkKWFrameWithLabel
{
public:
  static vtkKWMetaDataSetControlWidget* New();
  vtkTypeRevisionMacro(vtkKWMetaDataSetControlWidget,vtkKWFrameWithLabel);

  vtkGetObjectMacro (MetaDataSet, vtkMetaDataSet);
  virtual void SetMetaDataSet (vtkMetaDataSet* metadataset);

  vtkGetObjectMacro (ScalarColorFunction, vtkColorTransferFunction);
  
  virtual void Update();

  virtual void SetDataSetControlChangedCommand(
    vtkObject *object,const char *method);
  virtual void SetDataSetControlChangingCommand(
    vtkObject *object,const char *method);

  virtual void SetAssociatedObject (vtkObject* parent)
  {
    this->AssociatedObject = parent;
  }

  virtual void FitColorFunctionWithArray (vtkDataArray* array);
  
  
  
  // Description:
  // Callbacks
  virtual void OpacityChangingCallback(double op);
  virtual void ColorChangedCallback(double r, double g, double b);
  virtual void ColorChangingCallback(double r, double g, double b);
  virtual void SurfaceRepresentationToSurfaceCallBack   ();
  virtual void SurfaceRepresentationToWireframeCallBack ();
  virtual void SurfaceRepresentationToPointsCallBack    ();
  virtual void LineWidthChangingCallback (double width);
  virtual void InterpolationModeToFlatCallBack();
  virtual void InterpolationModeToGouraudCallBack();
  virtual void InterpolationModeToPhongCallBack();
  virtual void ComponentSelectionChangedCallBack();
  virtual void ScalarColorFunctionChangedCallback();
  virtual void ScalarColorPresetChangedCallback(const char* name);
  virtual void ReadDataButtonCallback();
  virtual void VolumePropertyButtonCallback();
  virtual void VolumePropertyChangedCallback ();
  virtual void VolumePropertyChangingCallback ();
  
  
  virtual void PropertyChangingCallback();
  virtual void PropertyChangedCallback();
  
  
  virtual void UpdateEnableState();



protected:
  vtkKWMetaDataSetControlWidget();
  ~vtkKWMetaDataSetControlWidget();

  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void Pack();

  virtual void SetProperty (vtkObject* property);
  virtual void InitializeColorFunction (void);
  
  


  virtual void InvokeObjectMethodCommand(const char *command);
  virtual void InvokeDataSetControlChangedCommand();
  virtual void InvokeDataSetControlChangingCommand();
  char* DataSetControlChangedCommand;
  char* DataSetControlChangingCommand;
  
  
  vtkKWFrameWithScrollbar*            MainFrame;
  vtkKWHSVColorSelector*              HSVColorSelector;
  vtkKWScale*                         OpacityScale;
  vtkKWSurfaceMaterialPropertyWidget* S_MaterialPropertyWidget;
  vtkKWVolumeMaterialPropertyWidget*  V_MaterialPropertyWidget;
  vtkKWPushButton*                    VolumePropertyButton;
  vtkKWRadioButtonSetWithLabel*       SurfaceRepresentationButtonSet;
  vtkKWScale*                         LineWidthScale;
  vtkKWRadioButtonSetWithLabel*       InterpolationModeButtonSet;
  vtkKWMenuButtonWithLabel*           ComponentSelectionButton;
  vtkKWColorPresetSelector*           ScalarColorSelector;
  vtkKWColorTransferFunctionEditor*   ScalarColorFunctionEditor;
  vtkKWPushButton*                    ReadDataButton;
  vtkKWVolumePropertyPresetSelector*  VolumePropertySelector;
  
  

  vtkMetaDataSet*                     MetaDataSet;  
  vtkProperty*                        S_Property;
  vtkVolumeProperty*                  V_Property;
  vtkColorTransferFunction*           ScalarColorFunction;
  
  
private:
  
  vtkKWMetaDataSetControlWidget(const vtkKWMetaDataSetControlWidget&);   // Not implemented.
  void operator=(const vtkKWMetaDataSetControlWidget&);                 // Not implemented.

  vtkObject* AssociatedObject;
  
};

#endif




