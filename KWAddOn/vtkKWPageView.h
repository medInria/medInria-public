/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWPageView.h 1302 2009-10-27 21:57:16Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-10-27 22:57:16 +0100 (Tue, 27 Oct 2009) $
Version:   $Revision: 1302 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkKWPageView_h
#define _vtkKWPageView_h

#include "vtkINRIA3DConfigure.h"

#include "vtkKWFrame.h"

#include <vector>
#include <string>

#include <vtkMetaImageData.h>
#include <itkImage.h>

class vtkKWRenderWidget;
class vtkKWIcon;
class vtkViewImage;
class vtkViewImage2D;
class vtkViewImage3D;
class vtkRenderWindow;
class vtkImageData;
class vtkDataSet;
class vtkLookupTable;
class vtkProperty;
class vtkActor;
class vtkPolyData;
class vtkLandmarkManager;
class vtkMatrix4x4;

class vtkViewImageCollection;


class KW_ADDON_EXPORT vtkKWPageView : public vtkKWFrame
{
public:
  static vtkKWPageView* New();
  vtkTypeRevisionMacro(vtkKWPageView,vtkKWFrame);


  //BTX
  enum GridTypeIds
  {
    GRIDTYPE_EQUAL_SIZE = 0,
    GRIDTYPE_3LEFT,
    GRIDTYPE_3BOTTOM,
    
  };
    
  /**
     The view mode allows allows to build a page view
     with view-with-tracer objects
  */
  enum View2DModeIds
  {
    VIEW_MODE_NORMAL = 0,
    VIEW_MODE_TRACING
  };
  //ETX

  
  void SetOrientationMatrix (vtkMatrix4x4* matrix);
  vtkMatrix4x4* GetOrientationMatrix (void)
  { return m_OrientationMatrix; }

  vtkViewImageCollection* GetPool (void) const
  { return m_Pool; }
  
  // Description:

  vtkGetMacro (IsFullScreen, int);
  vtkSetMacro (IsFullScreen, int);
  vtkBooleanMacro (IsFullScreen, int);
  void SetFullScreenView (int);
  
  vtkGetMacro(ID, int);
  vtkSetMacro(ID, int);
  
  vtkGetObjectMacro (View1, vtkViewImage2D);
  vtkGetObjectMacro (View2, vtkViewImage2D);
  vtkGetObjectMacro (View3, vtkViewImage2D);
  vtkGetObjectMacro (View4, vtkViewImage3D); 

  vtkGetObjectMacro (RenderWidget1, vtkKWRenderWidget);
  vtkGetObjectMacro (RenderWidget2, vtkKWRenderWidget);
  vtkGetObjectMacro (RenderWidget3, vtkKWRenderWidget);
  vtkGetObjectMacro (RenderWidget4, vtkKWRenderWidget);

/*   vtkGetObjectMacro (FiberManager, vtkFibersManager); */
  vtkGetObjectMacro (LandmarkManager, vtkLandmarkManager);

  

  
  virtual void          SetImage (vtkImageData* metaimage, vtkMatrix4x4* orientationmatrix = 0);
  //BTX
  virtual void          SetImage (itk::Image<float, 3>::Pointer image);
  //ETX
  virtual vtkImageData* GetImage (void);
  
  virtual void          ToggleFullScreenView4 (void);
  virtual void          ToggleFullScreenAxial (void);
  virtual void          ToggleFullScreenCoronal (void);
  virtual void          ToggleFullScreenSagittal (void);
  
  virtual int           GetAxialScreenshot(vtkKWIcon* screenshot, int size = 32);

  virtual void SetScalarBarVisibility (bool state);
  virtual int  GetScalarBarVisibility(void) const;
  virtual void ScalarBarVisibilityOn (void)
  { this->SetScalarBarVisibility (true); }
  virtual void ScalarBarVisibilityOff (void)
  { this->SetScalarBarVisibility (false); }

  virtual int  GetVisibility (vtkDataSet* dataset);
  virtual void SetVisibility (vtkDataSet* dataset, bool state);

  virtual void Render (void);

  /** Get/Set methods for the viewframe tag
      Use this tag in case you have several vtkKWPageView to manage */
  virtual const char* GetTag (void) const;
  virtual void        SetTag (const char* path);

  virtual void SetLookupTable (vtkLookupTable* lut);

  //BTX
  std::vector<vtkActor*> AddDataSet(vtkDataSet* dataset, vtkProperty* property);
  std::vector<vtkActor*> AddPolyData(vtkPolyData* dataset, vtkProperty* property = NULL, double thickness = 0.2);
  std::vector<vtkActor*> AddMetaDataSet(vtkMetaDataSet* metadataset);
  //ETX

  void RemoveDataSet(vtkDataSet* dataset);
  void RemoveMetaDataSet(vtkMetaDataSet* metadataset);

  vtkKWRenderWidget* GetActiveRenderWidget (void);
  vtkViewImage* GetActiveView (void);

  bool HasDataSet (vtkDataSet* dataset);
  
  vtkGetMacro (GridType, unsigned int);
  vtkSetMacro (GridType, unsigned int);
  
protected:
  vtkKWPageView();
  ~vtkKWPageView();

  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void CreateRenderWidgets();
  virtual void Create4Views();
  virtual void SetProperties();
  virtual void PackSelf();

  
  int BuildScreenshotFromImage(vtkImageData *image, vtkKWIcon* screenshot, int size = 0);
  int BuildScreenshotFromRenderWindow(vtkRenderWindow *win, vtkKWIcon* screenshot, int size = 0);

  void ConfigureView(vtkViewImage* view, vtkKWRenderWidget* widget);

  vtkMatrix4x4* m_OrientationMatrix;
    
  vtkViewImageCollection* m_Pool;
  

  vtkViewImage2D*      View1;
  vtkViewImage2D*      View2;
  vtkViewImage2D*      View3;
  vtkViewImage3D*      View4;
  vtkKWRenderWidget*   RenderWidget1;
  vtkKWRenderWidget*   RenderWidget2;
  vtkKWRenderWidget*   RenderWidget3;
  vtkKWRenderWidget*   RenderWidget4;


  vtkLandmarkManager* LandmarkManager;

  unsigned int GridType;


private:
  vtkKWPageView(const vtkKWPageView&);   // Not implemented.
  void operator=(const vtkKWPageView&);        // Not implemented.

  int ID;
  
  bool FirstRender;
  
  int               IsFullScreen;
  //BTX
  std::string Tag;
  //ETX
  
};

#endif
