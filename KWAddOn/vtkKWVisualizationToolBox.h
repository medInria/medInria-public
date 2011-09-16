/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWVisualizationToolBox.h 848 2008-05-21 10:10:47Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-05-21 11:10:47 +0100 (Wed, 21 May 2008) $
Version:   $Revision: 848 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkKWVisualizationToolBox_h
#define __vtkKWVisualizationToolBox_h

#include <vtkKWToolBox.h>

#include <vtkCommand.h>

#include <vector>

class vtkKWPushButton;
class vtkKWHSVColorSelector;
class vtkKWScale;
class vtkKWScaleSet;
class vtkMetaDataSet;
class vtkKWComboBox;
class vtkKWComboBoxSet;
class vtkKWComboBox;
class vtkKWMenuButton;
class vtkTensorVisuManager;
class vtkImageView;
class vtkKWSurfaceMaterialPropertyWidget;
class vtkScalarBarActor;
class vtkKWCheckButtonWithLabel;
class vtkKWFrameWithLabel;
class vtkKWRange;


class vtkKWVisualizationToolBox;

//BTX
class KW_ADDON_EXPORT vtkKWVisualizationCallback : public vtkCommand
{    
 public:
  
  static  vtkKWVisualizationCallback* New() 
    { return new vtkKWVisualizationCallback; }
  
  void Execute(vtkObject *   caller, 
               unsigned long event, 
               void *        callData);

  void SetToolbox (vtkKWVisualizationToolBox* tb)
  {
    this->Toolbox = tb;
  }

  void LockZoom (void)
  { this->ZoomLock = 1; }
  void UnLockZoom (void)
  { this->ZoomLock = 0;}

  void LockZ (void)
  { this->ZLock = 1; }
  void UnLockZ (void)
  { this->ZLock = 0;}

  void LockWindowLevel (void)
  { this->WindowLevelLock = 1; }
  void UnLockWindowLevel (void)
  { this->WindowLevelLock = 0;}

  
 protected:
  vtkKWVisualizationCallback()
  {
    this->Toolbox = NULL;
  };
  
  ~vtkKWVisualizationCallback(){};
  
 private:

  vtkKWVisualizationToolBox* Toolbox;
    

  
  int ZoomLock;
  int WindowLevelLock;
  int ZLock;  


 
  
};
//ETX

class KW_ADDON_EXPORT vtkKWVisualizationToolBox : public vtkKWToolBox
{
public:
  static vtkKWVisualizationToolBox* New();
  vtkTypeRevisionMacro(vtkKWVisualizationToolBox,vtkKWToolBox);

  virtual void ColorMapChangedCallback(const char* value);
  virtual void ButtonSetLookupTableCallBack();
  virtual void ButtonSetColorMapCallBack();
  virtual void OpacityChangingCallback(double op);
  virtual void ColorChangedCallback(double r, double g, double b);
  virtual void ColorChangingCallback(double r, double g, double b);
  virtual void LineWidthChangingCallback (double width);
  virtual void InterpolationModeChangedCallBack(const char* value);
  virtual void SurfaceRepresentationChangedCallBack (const char* value);
  virtual void ScalarbarsVisibilityCallback (int val);  
  virtual void ShadingCallback(int val);
  virtual void StanleyFunction (void);

  virtual void ImageRenderingModeCallback (const char* val);
  virtual void CroppingBoxCallback (int val);
  virtual void ImageScalarBarCallback (int val);
  virtual void ImageWindowLevelRangeCallback (double min, double max);
  virtual void MeshScalarRangeCallback (double min, double max);
  
  
  virtual void Update();

  vtkGetObjectMacro (ImageWindowLevelRange, vtkKWRange);

protected:
  vtkKWVisualizationToolBox();
  ~vtkKWVisualizationToolBox();

  // Description:
  // Create the widget.
  
  virtual void CreateMeshVisuFrame();
  virtual void CreateImageVisuFrame();
  virtual void CreateWidget();
  virtual void Pack();
  
  virtual void SetProperties();
  
  void PopulateDataList(void);

  vtkKWMenuButton* ButtonSetLookupTable;  
  vtkKWMenuButton* ButtonSetColorMap;  
  vtkKWComboBox*    DataListBox;
  vtkKWHSVColorSelector* HSVColorSelector;
  vtkKWScale*       OpacityScale;
  vtkKWScale*       LineWidthScale;
  vtkKWScaleSet*    ScaleSet;
  vtkKWComboBox*    SurfaceRepresentationComboBox;
  vtkKWComboBox*    InterpolationModeComboBox;
  vtkKWComboBoxSet* ComboBoxSet;
  vtkKWSurfaceMaterialPropertyWidget* S_MaterialPropertyWidget;
  vtkScalarBarActor* ScalarBar1;
  vtkScalarBarActor* ScalarBar2;
  vtkScalarBarActor* ScalarBar3;
  vtkScalarBarActor* ScalarBar4;
  vtkKWCheckButtonWithLabel* ScalarBarCheckbox;
  vtkKWCheckButtonWithLabel* ShadingCheckbox;
  vtkKWFrameWithLabel* MeshVisuFrame;
  vtkKWFrameWithLabel* ImageVisuFrame;

  vtkKWRange* ImageWindowLevelRange;
  vtkKWComboBox* ImageRenderingModeComboBox;
  vtkKWCheckButtonWithLabel* CroppingBoxCheckbox;
  vtkKWCheckButtonWithLabel* ImageScalarBarCheckbox;
  
  vtkKWRange* MeshScalarRange;

  unsigned int ColorMapId;
  unsigned int LUTId;
  
  vtkKWVisualizationCallback* Callback;
  


private:
  
  vtkKWVisualizationToolBox(const vtkKWVisualizationToolBox&);   // Not implemented.
  void operator=(const vtkKWVisualizationToolBox&);     // Not implemented.

  

};

#endif
