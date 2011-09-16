/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWMetaInfoWidget.h 612 2008-01-14 10:59:37Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-01-14 10:59:37 +0000 (Mon, 14 Jan 2008) $
Version:   $Revision: 612 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkKWMetaInfoWidget_h
#define __vtkKWMetaInfoWidget_h

#include "vtkINRIA3DConfigure.h"

#include "vtkKWFrame.h"
#include <string>
#include <vector>

class vtkKWMessageWithLabel;
class vtkKWPiecewiseFunctionEditorSet;
class vtkKWPiecewiseFunctionEditor;
class vtkKWHistogram;
class vtkKWRenderWidget;
class vtkImageView;

class vtkMetaDataSetSequence;
class vtkMetaDataSet;


class KW_ADDON_EXPORT vtkKWMetaInfoWidget : public vtkKWFrame
{
public:
  static vtkKWMetaInfoWidget* New();
  vtkTypeRevisionMacro(vtkKWMetaInfoWidget,vtkKWFrame);

  vtkGetObjectMacro(Sequence, vtkMetaDataSetSequence);

  void SetSequence(vtkMetaDataSetSequence* sequence);
  void SetMetaDataSet(vtkMetaDataSet* metadataset);
  

  
  // Description:
  // Callbacks. Internal, do not use.
  
protected:
  vtkKWMetaInfoWidget();
  ~vtkKWMetaInfoWidget();

  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void CreateAttributes();
  virtual void CreateHistograms();
  virtual void CreatePreviewTo2D();
  virtual void CreatePreviewTo3D();
  virtual void CreateRenderWidget();

  //virtual void SetHistogramsParameters();
  
  virtual void Pack();

  virtual void Update();
  virtual void UpdateAttributes();
  virtual void UpdateHistograms();
  virtual void UpdatePreview();

  virtual void AddHistogram(vtkKWHistogram* histogram, const char* name);
    
  
  
  vtkKWMessageWithLabel*  AttributesMessage;
  vtkKWPiecewiseFunctionEditorSet* HistogramsFrame;
  
  vtkMetaDataSetSequence* Sequence;
  vtkMetaDataSet* MetaDataSet;

  vtkKWRenderWidget*      RenderWidget;
  vtkImageView*           Preview;


  //BTX
  std::string AttributesText;
  //ETX
  

private:
  vtkKWMetaInfoWidget(const vtkKWMetaInfoWidget&);   // Not implemented.
  void operator=(const vtkKWMetaInfoWidget&);  // Not implemented.

  
  

};

#endif
