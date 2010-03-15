/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWSequenceImporter.h 1006 2008-11-13 15:58:19Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-11-13 16:58:19 +0100 (Thu, 13 Nov 2008) $
Version:   $Revision: 1006 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkKWSequenceImporter_h_
#define _vtkKWSequenceImporter_h_

#include "vtkINRIA3DConfigure.h"

#include <vtkKWMessageDialog.h>

class vtkMetaDataSetSequence;
class vtkKWSequenceEditorWidget;
class vtkKWPushButton;

class KW_ADDON_EXPORT vtkKWSequenceImporter: public vtkKWMessageDialog
{
 public:

  static vtkKWSequenceImporter* New();
  vtkTypeRevisionMacro(vtkKWSequenceImporter,vtkKWMessageDialog);
  
  void AddFromFileCallback();
  void AddFromManagerCallback(){};
  void ReadAttributesCallback();
  
  void SetSequence (vtkMetaDataSetSequence* sequence);
  vtkMetaDataSetSequence* GetSequence (void);

  virtual int  PreInvoke();
  virtual void PostInvoke();
  
 protected:
  vtkKWSequenceImporter();
  ~vtkKWSequenceImporter();
  
  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void PackSelf();


 private:
  
  vtkKWSequenceImporter(const vtkKWSequenceImporter&);      // Not implemented.
  void operator=(const vtkKWSequenceImporter&);        // Not implemented.

  vtkKWSequenceEditorWidget* SequenceEditorWidget;
  vtkKWPushButton*           ButtonAddFromFile;
  vtkKWPushButton*           ButtonReadAttributes;
  vtkKWPushButton*           ButtonAddFromManager;
  
};

#endif
