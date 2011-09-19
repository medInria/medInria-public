/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWTimeAnimationWidget.h 796 2008-04-16 17:25:01Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-04-16 18:25:01 +0100 (Wed, 16 Apr 2008) $
Version:   $Revision: 796 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkKWTimeAnimationWidget_h
#define __vtkKWTimeAnimationWidget_h

#include "vtkINRIA3DConfigure.h"

#include <vtkKWSimpleAnimationWidget.h>
#include <vector>


class vtkKWMainWindowInteractor;

class KW_ADDON_EXPORT vtkKWTimeAnimationWidget : public vtkKWSimpleAnimationWidget
{
public:
  static vtkKWTimeAnimationWidget* New();
  vtkTypeRevisionMacro(vtkKWTimeAnimationWidget,vtkKWSimpleAnimationWidget);

  
  // Description:
  // Set/Get the animation type.
  // If set to 'camera', the widget will display controls to rotate the
  // camera only the 3-axes.
  // If set to 'slice', the widget will display controls to iterate over
  // a range of slice. It is meant to actually slice through a 3D volume.
  // This 'slice' modes requires several callbacks to be also defined.
  //BTX
  enum 
  {
    AnimationTypeSequence  = 2
  };
  //ETX
  virtual void SetAnimationTypeToSequence()
  { this->SetAnimationType(
      vtkKWTimeAnimationWidget::AnimationTypeSequence);
  };

  virtual void SetAnimationType(int val);

  // Update the buttons' state
  virtual void Update();

  // Description:
  // Set the slice range (i.e. the indices of the first and last slices
  // available in the animation).
  virtual void SetSequenceRange(double min, double max, unsigned int size)
  {
    this->SequenceRange[0] = min;
    this->SequenceRange[1] = max;
    this->NumFrames        = size;
  }
  
  virtual void SetSequenceRange(const double range[2], unsigned int size) 
  { this->SetSequenceRange(range[0], range[1], size); };

  // Description:
  // Set the command to invoke to Update the sequence with the correct time flag
  // when the animation is in 'sequence' mode. 
  // This command is mandatory for the sequence animation to work.
  // The 'object' argument is the object that will have the method called on
  // it. The 'method' argument is the name of the method to be called and any
  // arguments in string form. If the object is NULL, the method is still
  // evaluated as a simple command. 
  // The following parameters are also passed to the command:
  // - the time value: double
  virtual void SetSequenceUpdateCommand(
    vtkObject *object, const char *method);


  // Callbacks : internal use only
  virtual void CreateAnimationCallback();
  virtual void PreviewAnimationCallback();
  
  
  
  void SetParentObject(vtkKWMainWindowInteractor* parent)
  { this->ParentObject = parent; }
  vtkKWMainWindowInteractor* GetParentObject (void)
  { return this->ParentObject; }
 
  
  virtual void GoAndBackCallback (int val);
  
  
protected:
  vtkKWTimeAnimationWidget();
  ~vtkKWTimeAnimationWidget();

  // Description:
  // Create the widget.
  virtual void CreateWidget();

  // Description:
  // Preview and create slice animation
  virtual void PreviewSequenceAnimation();
  virtual void CreateSequenceAnimation(
    const char *file_root, const char *ext, int width, int height);
  virtual void PerformSequenceAnimation(
    const char *file_root, const char *ext, int width, int height);

  virtual void InvokeSequenceUpdateCommand(double);
  char* SequenceUpdateCommand;
  
  vtkKWMainWindowInteractor* ParentObject;

private:
  
  vtkKWTimeAnimationWidget(const vtkKWTimeAnimationWidget&);   // Not implemented.
  void operator=(const vtkKWTimeAnimationWidget&);  // Not implemented.

  double SequenceRange[2];
  unsigned int NumFrames;

  bool GoAndBackMode;
  
  
  
  
};

#endif
