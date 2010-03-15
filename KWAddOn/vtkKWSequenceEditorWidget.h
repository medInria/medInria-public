/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWSequenceEditorWidget.h 477 2007-11-20 17:46:10Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-20 18:46:10 +0100 (Tue, 20 Nov 2007) $
Version:   $Revision: 1 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkKWSequenceEditorWidget_h
#define __vtkKWSequenceEditorWidget_h

#include "vtkINRIA3DConfigure.h"

#include <vtkKWFrame.h>
#include <string>
#include <vector>


class vtkMetaDataSetSequence;
class vtkMetaDataSet;

class vtkKWMultiColumnList;
class vtkKWSimpleEntryDialog;
class vtkKWEntry;
class vtkKWCheckButton;
class vtkKWPushButton;

class KW_ADDON_EXPORT vtkKWSequenceEditorWidget : public vtkKWFrame
{
 public:
  static vtkKWSequenceEditorWidget* New();
  vtkTypeRevisionMacro(vtkKWSequenceEditorWidget,vtkKWFrame);
  
  virtual void Initialize();
  
  void SetSequence (vtkMetaDataSetSequence* sequence)
  { this->Sequence = sequence; }
  vtkGetObjectMacro(Sequence, vtkMetaDataSetSequence);

  vtkGetMacro(AllowReordering, int);
  vtkSetMacro(AllowReordering, int);
  vtkBooleanMacro(AllowReordering, int);


  void SelectionChangedCallback();
  void CellUpdateCallback(int row, int col, double state);
  void DoubleClickCallback();
  void ResetCallback (void);
  void DurationCallback (const char* val);
  void TimeStampCallback (const char* val);
  void SameGeometryCallback (int val);
  void KeyPressDeleteCallback (void);
  void UpdateTimes (void);

  void TransposeDataToSequence (void);
  
  
 protected:
  vtkKWSequenceEditorWidget();
  ~vtkKWSequenceEditorWidget();

  
  vtkMetaDataSet* GetSelection (void);

  // Description:
  // Create the widget.
  virtual void CreateWidget();

  
 private:
  
  vtkKWSequenceEditorWidget(const vtkKWSequenceEditorWidget&);   // Not implemented.
  void operator=(const vtkKWSequenceEditorWidget&);                 // Not implemented.

  int AllowReordering;
  
  vtkKWMultiColumnList* MultiColumnList;
  
  vtkKWCheckButton* SameGeometryButton;
  vtkKWEntry*       DurationEntry;
  vtkKWEntry*       TimeStampEntry;
  
  vtkMetaDataSetSequence* Sequence;

  
  
};

#endif

