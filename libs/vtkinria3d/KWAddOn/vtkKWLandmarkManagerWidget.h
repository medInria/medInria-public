/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWLandmarkManagerWidget.h 993 2008-10-27 09:29:25Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-10-27 09:29:25 +0000 (Mon, 27 Oct 2008) $
Version:   $Revision: 993 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkKWLandmarkManagerWidget_h
#define __vtkKWLandmarkManagerWidget_h

#include "vtkINRIA3DConfigure.h"

#include <vtkKWFrame.h>
#include <vtkCommand.h>

class vtkKWPushButton;
class vtkKWHSVColorSelector;
class vtkKWScale;
class vtkKWComboBox;
class vtkKWFrameWithLabel;
class vtkKWLandmarkManagerWidget;
class vtkLandmarkManager;
class vtkLandmark;
class vtkKWCheckButtonWithLabel;


//BTX

class KW_ADDON_EXPORT vtkKWLandmarkManagerWidgetCallback : public vtkCommand
{
  
 public:
  static vtkKWLandmarkManagerWidgetCallback* New()
  { return new vtkKWLandmarkManagerWidgetCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long event, void* );  

  void SetLandmarkManagerWidget (vtkKWLandmarkManagerWidget* win)
  { this->LandmarkWidget = win; }
  
 protected:
  vtkKWLandmarkManagerWidgetCallback()
  {
    this->LandmarkWidget = 0;
  }
  ~vtkKWLandmarkManagerWidgetCallback(){}
  
  
 private:
  
  vtkKWLandmarkManagerWidget* LandmarkWidget;
};

//ETX

class KW_ADDON_EXPORT vtkKWLandmarkManagerWidget : public vtkKWFrame
{
public:
  static vtkKWLandmarkManagerWidget* New();
  vtkTypeRevisionMacro(vtkKWLandmarkManagerWidget,vtkKWFrame);

  virtual void ButtonAddCallback();
  virtual void ButtonDeleteCallback();
  virtual void ButtonDeleteAllCallback();
  virtual void ButtonLoadCallback();
  virtual void ButtonSaveCallback();
  virtual void SelectionChangedCallback(const char* value);
  virtual void ScalingChangingCallback(double op);
  virtual void ButtonCommentCallback(void);
  virtual void ColorChangingCallback(double r, double g, double b);
  virtual void GlobalScalingChangingCallback(double op);
  virtual void GlobalColorChangedCallback(double r, double g, double b);
  virtual void GlobalColorCodeChangedCallback(const char* value);
  virtual void GlobalLinkerVisibilityChanged(int val);
  
  virtual void Update();

  vtkGetObjectMacro(LandmarkManager, vtkLandmarkManager);
  void SetLandmarkManager (vtkLandmarkManager* manager);

  void PropertyChangeOff();

  void AddLandmarkToList (vtkLandmark* landmark);
  void RemoveLandmarkFromList (vtkLandmark* landmark);
  void HighlightLandmark (vtkLandmark* landmark);
  
  
protected:
  vtkKWLandmarkManagerWidget();
  ~vtkKWLandmarkManagerWidget();

  
  // Description:
  // Create the widget.
  
  virtual void CreateButtons();
  virtual void CreateWidget();
  virtual void Pack();

  vtkKWPushButton*       ButtonAdd;
  vtkKWPushButton*       ButtonDelete;
  vtkKWPushButton*       ButtonDeleteAll;
  vtkKWPushButton*       ButtonLoad;
  vtkKWPushButton*       ButtonSave;
  vtkKWComboBox*         LandmarkListBox;
  vtkKWHSVColorSelector* HSVColorSelector;
  vtkKWScale*            ScalingScale;
  vtkKWFrame*            ButtonFrame;
  vtkKWFrameWithLabel*   PropertyFrame;

  vtkKWFrameWithLabel*   GlobalPropertyFrame;
  vtkKWHSVColorSelector* GlobalHSVColorSelector;
  vtkKWScale*            GlobalScalingScale;
  vtkKWComboBox*         ColorCodeTypeListBox;
  vtkKWCheckButtonWithLabel* LinkerVisibilityCheckBox;
  
private:
  
  vtkKWLandmarkManagerWidget(const vtkKWLandmarkManagerWidget&);   // Not implemented.
  void operator=(const vtkKWLandmarkManagerWidget&);     // Not implemented.

  vtkLandmarkManager* LandmarkManager;
  vtkKWLandmarkManagerWidgetCallback* Callback;
  
  
  

};

#endif
