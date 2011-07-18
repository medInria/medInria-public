/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWPreviewPage.h 560 2007-12-17 13:20:51Z ntoussaint $
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
#ifndef _vtkKWPreviewPage_h
#define _vtkKWPreviewPage_h

#include "vtkINRIA3DConfigure.h"

#include "vtkKWFrame.h"
#include <vtkCollection.h>

#include <vector>
#include <string>

#include <vtkImageView3D.h>
#include <vtkImageViewCollection.h>

class vtkKWRenderWidget;
class vtkKWIcon;
class vtkRenderWindow;
class vtkImageData;
class vtkDataSet;
class vtkLookupTable;
class vtkProperty;
class vtkActor;
class vtkPolyData;
class vtkKWFrameWithScrollbar;
class vtkKWWidgetSet;
class vtkMatrix4x4;



class KW_ADDON_EXPORT vtkKWPreviewPage : public vtkKWFrame
{
public:
  static vtkKWPreviewPage* New();
  vtkTypeRevisionMacro(vtkKWPreviewPage,vtkKWFrame);
  
  // Description:

  virtual void AddPreviewImage (vtkImageData* image, const char* name, vtkMatrix4x4* matrix = 0);
  virtual void RemovePreviewImage (vtkImageData* image);
  
  virtual void Render (void);

  vtkGetMacro (LinkViews, int);
  vtkSetClampMacro (LinkViews, int, 0, 1);
  vtkBooleanMacro (LinkViews, int);

  vtkGetMacro (OrientationMode, int);
  void SetOrientationMode (int mode);

  vtkGetMacro (InteractionMode, int);
  void SetInteractionMode (int mode);
  

  vtkGetMacro (MaxNumberOfColumns, int);
  vtkSetMacro (MaxNumberOfColumns, int);
  
  vtkGetObjectMacro (LookupTable, vtkLookupTable);
  void SetLookupTable (vtkLookupTable* lut);


  unsigned int GetNumberOfPreviews (void)
  {
    return this->ViewList->GetNumberOfItems() - 1;
  }

  virtual void Update (void);
  
  void SetEnableViews(unsigned int arg);
  vtkBooleanMacro (EnableViews, unsigned int);
  
  
  
protected:
  vtkKWPreviewPage();
  ~vtkKWPreviewPage();

  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void PackSelf();

  virtual vtkImageView2D* FindView(vtkImageData* imagedata, int &cookie);
  virtual void ConfigureView(vtkImageView* view, vtkKWRenderWidget* widget);

private:
  vtkKWPreviewPage(const vtkKWPreviewPage&);   // Not implemented.
  void operator=(const vtkKWPreviewPage&);        // Not implemented.

  vtkImageViewCollection* ViewList;
  vtkImageView3D* GlobalView;
  vtkKWRenderWidget* GlobalRenderWidget;
  
  vtkCollection* RenderWidgetList;
  vtkKWWidgetSet* WidgetGrid;
  
  vtkLookupTable* LookupTable;
  int LinkViews;
  int OrientationMode;
  int MaxNumberOfColumns;
  int InteractionMode;
  
  vtkKWFrameWithScrollbar* InternalFrame;

  
  
};

#endif
